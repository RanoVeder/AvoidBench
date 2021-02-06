import avoidbench
import time
import parameters
import cv2
import numpy as np
import pdb
import pickle
from PIL import Image
import random


def get_optical_flow(prev_bgr, bgr, max_points=100, DEBUG=False):
    """
    Created on Fri Jun 28 18:34:27 2019

    Determine Lucas-Kanade optical flow with FAST feature points. 

    @author: Guido de Croon, TU Delft.
    """
    
    # Initiate FAST object with default values
    is_cv3 = cv2.__version__.startswith("3.") or cv2.__version__.startswith("4.")
    if not is_cv3:
        # if(os.name != 'nt'):
        fast = cv2.FastFeatureDetector()
    else:
        fast = cv2.FastFeatureDetector_create()
    # find and draw the keypoints
    kp = fast.detect(prev_bgr,None)

    # downselect the points:
    kp = np.random.choice(kp, size=max_points)
    n_points = len(kp)
    if DEBUG:
        img2 = bgr.copy()
        cv2.drawKeypoints(bgr, kp, img2, color=(255,0,0))
        cv2.imshow('img2', img2); # [:,:,0:3]
        cv2.waitKey(10)
    
    # convert the points to a 2D numpy array:
    currPts = np.float32(np.zeros([n_points, 2]))
    for i in range(n_points):
        currPts[i, 0] = kp[i].pt[0]
        currPts[i, 1] = kp[i].pt[1]

    # cv2.calcOpticalFlowPyrLK(prevImg, nextImg, prevPts, nextPts, status, err, winSize, maxLevel, criteria, flags, minEigThrshold)
    # we will probably call it like this:
    prev_grayscale = cv2.cvtColor(prev_bgr, cv2.COLOR_BGR2GRAY)
    grayscale =  cv2.cvtColor(bgr, cv2.COLOR_BGR2GRAY)
    nextPts = np.float32(np.zeros([n_points,2]))
    nextPts, status, err = cv2.calcOpticalFlowPyrLK(prev_grayscale, grayscale, currPts, nextPts, maxLevel=2)
    return currPts, nextPts, status

class FactoryBenchmark(avoidbench.BenchmarkingInterface):


    def __init__(self, ip, port):
        poisson_radii = [400, 350, 300, 250, 200]
        benchmark_map = "/Game/Maps/UnitTestMaps/factory"
        procedural_locations = ["Main_L0", "Main_L1", "Main_L2", "Main_L3", "Main_L4", "Main_L5"]
        mission_name = "Mission_Main"

        avoidbench.BenchmarkingInterface.__init__(self, ip, port)


        #Create benchmarking tasks with different poisson radii, seeds and obstacles(procedural_location)
        for procedural_location in procedural_locations:
            for poisson_radius in poisson_radii:
                for seed in range(10):

                    loc = avoidbench.ProceduralGenerationDescription()
                    loc.procedural_location = procedural_location
                    loc.radius = poisson_radius
                    loc.seed = seed
                    
                    
                    lvl = avoidbench.BenchmarkingDescription()
                    lvl.world_name = benchmark_map
                    lvl.mission_name = mission_name
                    lvl.trials = 3
                    lvl.procedural_descriptions = [loc]


                    self.add_benchmarking_description(lvl)


class BenchmarkImplementation(FactoryBenchmark):
    def __init__(self, ip, port):
        super().__init__(ip, port)

    def _init(self):
        self.first = True
        self.lateral_control = 0
        self.vertical_control = 0
        self.lateral_control_active = True
        self.lateral_control_timer = time.time()*1000

        self.last_time = 0
        self.reset_timer = 0

    def _tick(self, vehicle, mission):

        current_time = time.time()
        elapsed_time = current_time - self.last_time

        #ensure we run at 10Hz
        while(elapsed_time < 0.1):
            current_time = time.time()
            elapsed_time = current_time - self.last_time

        cont = avoidbench.VelocityControllerReference()
        #Calculate goal heading
        state = vehicle.get_state()
        yaw = np.arctan2(mission.end_position[1] - state.position[1], mission.end_position[0] - state.position[0])
        print(3)
        #Obtain FPV image
        image = vehicle.get_image("fpv")
        print(4)
        width = image.width
        height = image.height

        #Sometimes Unreal engine fails to send an image. This is a temporary solution
        if width == 0:
            return
        
        #Resize image
        dim = (width, height)
        img = np.array(image.data, np.uint8).reshape(height, width, 3)
        frame = cv2.resize(img, dim, interpolation = cv2.INTER_AREA)

        #Crop Image
        ROI = [120, 30]
        ROIC = [int(width/2.), int(height/2.)]
        frame = frame[ (ROIC[1] - int(ROI[1]/2)): (ROIC[1] + int(ROI[1]/2)), (ROIC[0] - int(ROI[0]/2)): (ROIC[0] + int(ROI[0]/2)),].copy()

        #Save the first image we receive
        if(self.first == True):
            self.original_yaw = yaw
            self.previous_image = frame
            self.first = False 
            self.last_time = current_time
            return


        #calculate optical flow
        try:
            currPts, nextPts, status = get_optical_flow(self.previous_image, frame, 150, False)
        except:
            return
        flow = nextPts - currPts


        #Show received image
        f_copy = frame.copy()
        cv2.imshow('mono', f_copy); # [:,:,0:3]
        cv2.waitKey(1)


        #check if the drone is executing a control task.
        if self.lateral_control_active == True:
            now = time.time()*1000
            diff = now-self.lateral_control_timer
            if diff > 500:
                self.lateral_control_active = False
                self.lateral_control = 0
                self.vertical_control = 0
                self.reset_timer = now
        
        #calculate total optical flow in left and right half of image
        left_mag = 0
        right_mag = 0
        
        #TODO: normalize optical flow in the future
        for p0, p1 in zip(currPts, nextPts):
            magnitude = np.sqrt((p0[0] - p1[0])**2 + (p0[1] - p1[1])**2)
            if(p0[0] > (width/2)):
                right_mag += magnitude
            else:
                left_mag += magnitude
    

        #Check for obstacle
        if self.lateral_control_active == False and (time.time()*1000 - self.reset_timer) > 1300 : 
            if  left_mag > 150 and abs(left_mag - right_mag) > 60:
                self.lateral_control = 1.5
                self.vertical_control = 0.15
                self.lateral_control_active = True
                self.lateral_control_timer = time.time()*1000
            elif right_mag > 150 and abs(left_mag - right_mag) > 60:
                self.lateral_control = -1.5
                self.vertical_control = 0.15
                self.lateral_control_active = True
                self.lateral_control_timer = time.time()*1000


        #Apply control references
        cont.vy = self.lateral_control
        cont.vz = self.vertical_control
        cont.vx = 1
        cont.yaw = yaw

        print(5)
        vehicle.apply_controller_reference(cont)
        print(6)
        #save image
        self.previous_image = frame
        self.last_time = current_time



    def _cleanup(self):
        pass


x = BenchmarkImplementation("localhost", 8090)
x.run("outputFolder")

import avoidbench

import time
import math
import cv2
import numpy as np

#Connect to the simulator
client = avoidbench.Client("localhost", 8090)

#Get available worlds and connect to the random forest
print(client.get_available_worlds())
w = client.load_world("/Game/Maps/UnitTestMaps/forest_flat_random")

#Get the available procedural forests, and generate a forest with a Poisson Disc radius of 10 meter
print(w.get_procedural_locations())
w.set_procedural_location("Forest_Full", 1000, 2)

#Get the available missions
print(w.get_available_missions())

#Load and start the "Mission_Full_Straight" mission
mission = w.load_mission("Mission_Full_Straight")
mission.start()

#Obtain the vehicle (MAV)
vehicle = client.get_vehicle()


while True:
    #Obtain the FPV image and convert it to a numpy array
    image = vehicle.get_image("fpv")
    img = np.array(image.data, np.uint8).reshape(image.height, image.width, 3)

    #Get the mission_info so that we can calculate the heading to the goal
    mission_info = mission.get_mission_info()

    #Get the current position of the drone
    state = vehicle.get_state()
    yaw = np.arctan2(mission_info.end_position[1] - state.position[1], mission_info.end_position[0] - state.position[0])


    #Set the reference controller state and apply it
    ref = avoidbench.VelocityControllerReference()
    ref.yaw = yaw
    ref.vx = 1
    ref.vy = 0
    vehicle.apply_controller_reference(ref)

	#Show the FPV image
    cv2.imshow('FPV', img)
    cv2.waitKey(15)
   
import os
import cv2
import numpy as np

from camera import *
from grids import *
from motion_generator import *

def line_equation_vector(point1, point2):
    a = point2[1] - point1[1]
    b = -(point2[0] - point1[0])
    c = (point2[0] - point1[0]) * point1[1] - (point2[1] - point1[1]) * point1[0]
    return np.array([a, b, c])


num_cols = 12
point0 = num_cols * 6 + 3
point1 = num_cols * 6 + 6
point2 = num_cols * 3 + 3
point3 = num_cols * 3 + 6

def affine_rectification(points):
    rectified_points = []  # Initialize the rectified points
    # points is an array coordinate
    # TODO #1: Assign four points of first parallel lines on the image 'projectivity', two of each line.
    #      (Hint: you can choose the points from the dump image or print projective points at specific row and column)
    #      (Note: Draw selected points on image 'projectivity' and attach them in your report)

    chosen_points = [points[point0], points[point1],
                     points[point2], points[point3]]
    
    # TODO #2: Compute their vanishing point (the intersection point of lines on 'projectivity'.
    #      (Note: Write inhomogeneous coordinate of vanishing point in your report)

    # First parallel lines: (6, 3) with (3, 3) and (6, 6) with (3, 6)
    first_parallel_lines_1 = line_equation_vector(chosen_points[0], chosen_points[2])
    first_parallel_lines_2 = line_equation_vector(chosen_points[1], chosen_points[3])

    first_vanishing_point = np.cross(first_parallel_lines_1, first_parallel_lines_2)
    first_vanishing_point = (first_vanishing_point / first_vanishing_point[2])[:2]
    print(f"First vanishing point: {first_vanishing_point}")

    # TODO #3: Similarly, assign four points of second parallel lines and compute their vanishing point.
    #      (Note: Provide your selected points and inhomogeneous coordinate of second vanishing point in a similar manner)

    # Second parallel lines: (6, 3) with (6, 6) and (3, 3) with (3, 6)
    second_parallel_lines_1 = line_equation_vector(chosen_points[0], chosen_points[1])
    second_parallel_lines_2 = line_equation_vector(chosen_points[2], chosen_points[3])

    second_vanishing_point = np.cross(second_parallel_lines_1, second_parallel_lines_2)
    second_vanishing_point = (second_vanishing_point / second_vanishing_point[2])[:2]
    print(f"Second vanishing point: {second_vanishing_point}")

    # TODO #4: Determine the line at infinity l' on the image by connecting two vanishing point
    #      (Note: please report line at infinity with 3rd coordinate = 1)

    infinity_line = line_equation_vector(first_vanishing_point, second_vanishing_point)
    infinity_line = (infinity_line / infinity_line[2])
    print(f"Line at infinity: {infinity_line}")

    # TODO #5: Rectify all points by rectified_points = H * points, where H: [1 0 0; 0 1 0; l_1/l_3 l_2/l_3 1], and l'=[l_1; l_2; l_3]

    H = np.array([[1, 0, 0],
                  [0, 1, 0],
                  [infinity_line[0], infinity_line[1], 1]], dtype=np.float64)
    for point in points:
        homogeneous_point = np.array([point[0], point[1], 1.0], dtype=np.float64)
        rectified_points.append(np.matmul(H, homogeneous_point))

    #  The operation is in terms of 'homogeneous', and the rectified point should be scaled such that the 
    #       third component is '1' for 'inhomogenous result'). 
    for idx, rectified_point in enumerate(rectified_points):
        rectified_point /= rectified_point[2]
        rectified_point = rectified_point[:2].astype(int)
        rectified_points[idx] = rectified_point

    # TODO #6: Return the rectified points instead of points
    return rectified_points


# Create object
grids = Grids()
points = grids.generate_points()

# Create camera
fov, image_w, image_h = 60, 800, 600
camera = Camera(fov, image_w, image_h)

# Points after similarity
# 1. Project the grids without pre-rotation 
similarity_points = []
for point in points:
    similarity_points.append(camera.project_to_image_position(point))

# Points after projective
# 1.Rotate the grids
# Generate rotation along x-axis and z-axis
theta_x_deg = 15  # Rotation angle in degree
theta_y_deg = 15  # Rotation angle in degree
Rx = compute_rotation_x(np.radians(theta_x_deg))
Ry = compute_rotation_y(np.radians(theta_x_deg))
rotated_points = []
for point in points:
    rotated_points.append(Ry @ Rx @ np.array(point))
# 2.Project the grids to the image plane
projective_points = []
for point in rotated_points:
    projective_points.append(camera.project_to_image_position(point))

# Affine rectification for the projected points
affine_points = affine_rectification(projective_points)

# Render the grid image
# Generate line indices to connect the points    
line_indices = grids.generate_line_indices()
color = [0, 255, 0]  # Line color
thickness = 3  # Line thickness
# Draw lines
similarity_image = np.zeros([image_h, image_w, 3], dtype=np.uint8)
affine_image = np.zeros([image_h, image_w, 3], dtype=np.uint8)
projective_image = np.zeros([image_h, image_w, 3], dtype=np.uint8)

for pair in line_indices:
    p, q = pair[0], pair[1]
    cv2.line(similarity_image, similarity_points[p], similarity_points[q], color, thickness)
    if p < len(affine_points) and q < len(affine_points):
        cv2.line(affine_image, affine_points[p], affine_points[q], color, thickness)
    cv2.line(projective_image, projective_points[p], projective_points[q], color, thickness)

# Draw Choosing Point on projective_points
cv2.circle(projective_image, projective_points[point0], radius=5, color=(0,0,255), thickness = -1)
cv2.circle(projective_image, projective_points[point1], radius=5, color=(0,0,255), thickness = -1)
cv2.circle(projective_image, projective_points[point2], radius=5, color=(0,0,255), thickness = -1)
cv2.circle(projective_image, projective_points[point3], radius=5, color=(0,0,255), thickness = -1)

# Dump the projected image
# Create output folder if not exist
folder_out = "results"
if not os.path.exists(folder_out):
    os.makedirs(folder_out)
# Write image to the folder
cv2.imwrite(folder_out + '/similarity.png', similarity_image)
cv2.imwrite(folder_out + '/projectivity.png', projective_image)
cv2.imwrite(folder_out + '/affinity.png', affine_image)

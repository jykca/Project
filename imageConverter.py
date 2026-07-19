import cv2
import numpy

imageFile = 'images/box.jpg'
txtFile = 'paths/box.txt'

## Read an image
image = cv2.imread(imageFile)
image = cv2.resize(image, (1000, 1000))

## Convert to grayscale
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

## blur the image to keep outer edges
blur = cv2.GaussianBlur(gray, (7,7), 0)

## Apply Canny edge detection
edges = cv2.Canny(blur, 100, 200)

## Find contours
contours, hierarchy = cv2.findContours(
    edges,
    cv2.RETR_EXTERNAL,
    cv2.CHAIN_APPROX_SIMPLE
)

## Create black canvas to put simplified contours on
output = numpy.zeros_like(edges)

##cv2.drawContours(output, contours, -1, 255, 2)

## Clear file
with open(txtFile, "w") as f:
    pass

for contour in contours:
    ## A measure of how much simplification
    epsilon = 0.01 * cv2.arcLength(contour, True)

    approx = cv2.approxPolyDP(contour,epsilon,True)

    cv2.drawContours(output, [approx], -1, 255, 2)

    points = approx.reshape(-1, 2)

    for i in range(len(points)):
        start = points[i]
        end = points[(i+1) % len(points)]

        print("Line:")
        print("Start:", start)
        print("End:", end)

        with open(txtFile, "a") as f:
            convertedStart = numpy.round((start / 50) - 10,2)
            convertedEnd = numpy.round((end / 50) - 10,2)
            f.write("0;" + str(convertedStart[0]) + "," + str(convertedStart[1]) + ";" + str(convertedEnd[0]) + "," + str(convertedEnd[1]) + "\n")

# Display the image
cv2.imshow("Outline", output)
cv2.waitKey(0)
cv2.destroyAllWindows()
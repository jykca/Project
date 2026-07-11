import cv2
import numpy

# Read an image
image = cv2.imread('images/apple.jpg')

# Convert to grayscale
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# blur the image to keep outer edges
blur = cv2.GaussianBlur(gray, (7,7), 0)

# Apply Canny edge detection
edges = cv2.Canny(blur, 100, 200)

contours, _ = cv2.findContours(
    edges,
    cv2.RETR_EXTERNAL,
    cv2.CHAIN_APPROX_SIMPLE
)

output = numpy.zeros_like(edges)

cv2.drawContours(output, contours, -1, 255, 2)

# Display the image
cv2.imshow("Outline", output)
cv2.waitKey(0)
cv2.destroyAllWindows()
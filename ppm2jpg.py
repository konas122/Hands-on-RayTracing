import cv2
import argparse


parser = argparse.ArgumentParser(description='ppm2jpg')
parser.add_argument('input_file', type=str, default='Test.ppm',
                    help='input the path of input file')
parser.add_argument('output_file', type=str, default='output.jpg',
                    help='input the way of output file')
args = parser.parse_args()

img = cv2.imread(args.input_file, cv2.IMREAD_COLOR)
cv2.imwrite(args.output_file, img, [int(cv2.IMWRITE_JPEG_QUALITY), 95])

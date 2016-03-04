# dark_frame_sub

Simple dark frame subtraction program.

## Description

dark_frame_sub is a simple program that imports your night sky phtography and dark frame(s) taken at the same time to generate less noise image.
This program is especially good for erase camera specific noise such as hot pixels.
About dark frame subtraction, refer to wiki page.

[Wiki](https://en.wikipedia.org/wiki/Dark-frame_subtraction)

Before

<img src="https://raw.githubusercontent.com/delphinus1024/dark_frame_sub/master/before.png" style="width: 600px;"/>

After

<img src="https://raw.githubusercontent.com/delphinus1024/dark_frame_sub/master/after.png" style="width: 600px;"/>

## Features

- Imports night sky photo and dark frame photo(s),  average the dark frames to generate reference dark frame image , subtract the reference dark frame image from the night sky photo.
- The reference dark frame image can be saved so that you can apply it to another photo(s) taken at the same occation to save averaging time.
- Floating point calculation is possible, takes a long calculation time with slight improvement though.

## Requirement

- OpenCV 3.0.0 or above is preferable.
- Boost C++ serialization and filesystem (I checked with version 1.60.0)
- I Checked with win7 32bit + msys2 + gcc

## Usage

1. Prepare a photo to be processed.
2. Prepare dark frame photo(s) and store them into a same directory. No other files should not exist in the direcotry since this program will regard all files in the directory as dark frames.
3. In the working directory, type following command in your command window.

 dark_frame_sub.exe [params] image out_image dark_dir

        -?, -h, --help, --usage (value:true)
                show help
        -D, -d, --savedark
                save dark image
        -F, -f, --float
                do float calculation
        -S, -s, --show
                show result
        -U, -u, --usedark
                use dark mat if exist
        image
                input image file
        out_image
                output image file
        dark_dir
                dark image folder
4. U option saves the reference dark frame image in cv::Mat format when this file (generated in the past by this program) is not found in the working directory. On the other hand, this option will import the reference dark frame image file if exists and ignores dark_dir. So it will reduce the average operation time from the second photos when multiple photos are processed in batch. The filename of the referece dark frame image is fixed.
5. F option will use floating point calculation to improve the precision, will take long calculation time.
6. S option will show the original photo ,the reference dark frame and the result when finished.
7. D option will save the reference dark frame image to tiff format. Fixed filename, dark.tif.
8. "image" is filename of the photo to be processed.
9. "out_image" is filename of the photo after processed.
10. "dark_dir" is the directory name of the dark frames you stored at the step 2. This option is not needed when -U option is specified and the reference dark frame image file exists. Otherwise, this option is mandatory.

## Installation

1. Modify Makefile according to your OpenCV and Boost inludes and libs environment.
2. make

## Author

delphinus1024

## License

[MIT](https://raw.githubusercontent.com/delphinus1024/dark_frame_sub/master/LICENSE.txt)


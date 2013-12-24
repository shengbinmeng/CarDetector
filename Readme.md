#Intro

./CarDetector.sln contains two projects, CarDetetor and SVMGenerator, both depending on OpenCV2.0, of which the headers and libs has already been put in ./OpenCV2.0.

SVMGenerator is to train the SVM classifier, and will save the SVM as an xml file. CarDetector is a GUI program to detect cars, and it needs the xml file generated by SVMGenerator.

./executable/ is the directory where the program is built to and runs. The file reading and writing of the program will be in the directory where itself is in.

#Build

Open the .sln with VS2008, and build(F7). SVMGenerator.exe and CarDetetor.exe will be generated in ./executable. (The binary name will have a suffix of 'd' for the debug version.)

#Run

To run, the .exe files need OpenCV2.0 DLLs, which are already in ./executable. Please run them in this directory, otherwise you have to make sure the DLLs can be found(e.g. in PATH).

CarDetetor.exe will read CarSVM.xml in its own directory. A usable xml file has been put in ./executable.
If you want to train the SVM to generate CarSVM.xml again, run the SVMGenerator.exe, and pass it the path to the training set picture folder.

Use CarDetetor.exe as the GUI directs you. A simple evaluation process is: select the test set picture folder, hit Detect All, then hit Evaluate when the progress bar finishes.
The Evaluate command needs java.exe in your system path.

#More

The progress bar in this GUI program is based on http://www.xiaohui.com/dev/vccool/progress/2.htm. OpenCV can be downloaded at: http://www.opencv.org.cn/index.php/Download.
The car data set is from: http://cogcomp.cs.illinois.edu/Data/Car/.

Any problem, please contact: shengbinmeng@gmail.com.


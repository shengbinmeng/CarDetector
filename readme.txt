# 简介

./CarDetector.sln为VS2008解决方案文件。该解决方案包含两个项目，CarDetetor和SVMGenerator，二者均依赖OpenCV2.0，所需的头文件和库文件已经放在./OpenCV2.0目录下。
SVMGenerator项目用于训练SVM分类器，将训练出的分类器以xml文件的形式保存下来。CarDetector项目为车辆检测的GUI程序，需要用到SVMGenerator生成的xml文件。./executable/为可执行程序存放和运行的目录，程序储存和和读取文件的路径都是其自身所在目录。

# 生成

用VS2008打开解决方案并生成，将会在./executable目录下生成SVMGenerator.exe和CarDetetor.exe（如果是debug版本，则对应文件名后多一个字母d，下同）。

# 运行

exe文件运行时需要OpenCV2.0的动态链接库，对应的dll文件已经放在./executable目录下，请按照默认在该目录下运行程序，否则就需要将这些dll添加到系统PATH中。
CarDetetor.exe是车辆检测的主程序，需要在其所在目录下读取CarSVM.xml（可用的xml文件已经放在了./executable下）。
如要重新训练SVM得到CarSVM.xml，可运行SVMGenerator.exe，并将训练集图片文件夹的路径作为唯一传入参数。
对于CarDetetor.exe的使用，按照界面操作。简单的评测过程如下：选择对应测试图片文件夹，点击Detect All，待进度完成后点击Evaluate。Evaluate命令需要在系统路径下有可用的java.exe。

# 其他

GUI中的进度条窗口使用了网上开源代码，下载地址为：http://www.xiaohui.com/dev/vccool/progress/2.htm，OpenCV开源代码下载地址为：http://www.opencv.org.cn/index.php/Download。

有任何问题可联系:shengbinmeng@gmail.com.


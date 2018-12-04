# OOP-Lecture-Project
## 负责前端的同学看这里~
工程里的其他文件都不重要，请复制以下文件到你们的GUI中：

* ImageConverter.cpp
* ImageConverter.h
* ImageProcess.cpp
* ImageProcess.h

实际使用时，将ImageConverter.cpp修改成没有什么实际作用的东西，ImageProcess.cpp也可以换成空文件<br>
GUI端写好应该调用的函数（也就是在程序中暂时省略OpenCV相关的函数实现）<br>
在以上操作后，你们应该不需要OpenCV库就可以运行了<br>
具体请看ImageProcess.h内的说明<br>
也可以试试把GUI一起push上来，不过我用的是VS的.sln，如果你们是Qt creator可能打不开

## 负责后端的同学看这里~
工程里的ImageConverter不重要，不用管它<br>
新功能的实现尽量不要直接写在ImageProcess.cpp里（例程那个是我懒），而是将ImageProcess类的函数都作为调用实际实现功能的函数的入口<br>
ImageProcess.h的include内容不要修改，新include项请写在cpp文件里<br>
然后向着ImageProcess类加功能就行了

# FishEyeRemaper
This xcode C++ (with OpenCV) project is for transforming Fisheye image to expanded map(Plate Carree's method). 

## Usage
This is a tiny C++ program with **XCODE**. It's easy to use it in **Linux** with Makefile or **Windows** with VisualStudio.

* OpenCV is required(version is not limited)

* Fisheye lens parameters(Params Func) are required


#### Fisheye Lens Params Example

> * Distortion Type: F-Tan(Theta).
> * Height units are `Millimeters`.
> * Maximum Field is `89.000` Degrees.
> * Distortion focal length = `1.30651`

|Y Angle(deg)|Real Height|
|:----------:|:---------:|
|  0.000000  |  0.000000 |
|  0.890000  |  0.020295 |
|    ...     |    ...    |
|  88.11000  |  1.742611 |
|  89.00000  |  1.750281 |

#### Fisheye len model
![fish eye model](https://raw.githubusercontent.com/changkaizhao/changkaizhao.github.io/master/images/FisheyeRemaper/fisheye model.png)

Using polynomial fit method,we get fit function as below:

         $$ h = a + b*y + c*y^2 + d*y^3 + e*y^4 + f*y^5 + g*y^6 $$




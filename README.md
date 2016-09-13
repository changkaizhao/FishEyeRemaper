# FishEyeRemaper
This xcode C++ (with OpenCV) project is for transforming Fisheye image to expanded map(Plate Carree's method). 

## Usage
This is a tiny C++ program with **XCODE**. It's easy to use it in **Linux** with Makefile or **Windows** with VisualStudio.

* OpenCV is required(version is not limited)
* Fisheye lens parameters(Params Func) are required
* CMOS size is required


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

>(LaTeX) \[h = a + b*y + c*y^{2} + d*y^{3} + e*y^{4} + f*y^{5} + g*y^{6}\]

<math xmlns="http://www.w3.org/1998/Math/MathML">
<mtable class="m-equation-square" displaystyle="true" style="display: block; margin-top: 1.0em; margin-bottom: 2.0em">
	<mtr>
		<mtd>
			<mspace width="6.0em" />
		</mtd>
		<mtd columnalign="left">
			<mi>h</mi>
			<mo>=</mo>
			<mi>a</mi>
			<mo>+</mo>
			<mi>b</mi>
			<mo>*</mo>
			<mi>y</mi>
			<mo>+</mo>
			<mi>c</mi>
			<mo>*</mo>
			<msup>
				<mi>y</mi>
				<mn>2</mn>
			</msup>
			<mo>+</mo>
			<mi>d</mi>
			<mo>*</mo>
			<msup>
				<mi>y</mi>
				<mn>3</mn>
			</msup>
			<mo>+</mo>
			<mi>e</mi>
			<mo>*</mo>
			<msup>
				<mi>y</mi>
				<mn>4</mn>
			</msup>
			<mo>+</mo>
			<mi>f</mi>
			<mo>*</mo>
			<msup>
				<mi>y</mi>
				<mn>5</mn>
			</msup>
			<mo>+</mo>
			<mi>g</mi>
			<mo>*</mo>
			<msup>
				<mi>y</mi>
				<mn>6</mn>
			</msup>
		</mtd>
	</mtr>
</mtable>
</math>


####Bilinear Interpolation
![Lookup table](https://raw.githubusercontent.com/changkaizhao/changkaizhao.github.io/master/images/FisheyeRemaper/chart.png)

we use bilinear interpolation method to get ouput values.Details show in the chart above.

####Two types Remaping
In remaper,two expanding ways(sphere and barrel) are implemented.


###If you have any questions please feel free to contact me or report issues.
>Email:changkaizhao1006@gmail.com





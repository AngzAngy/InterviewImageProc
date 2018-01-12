precision mediump float;
varying vec2 vTextureCoord;
uniform sampler2D sTexture;
uniform vec2 imageSize;
uniform float level;
void main() {
vec4 sum;
vec2 offset0=vec2(-1.0,-1.0); vec2 offset1=vec2(0.0,-1.0); vec2 offset2=vec2(1.0,-1.0);
vec2 offset3=vec2(-1.0,0.0); vec2 offset4=vec2(0.0,0.0); vec2 offset5=vec2(1.0,0.0);
vec2 offset6=vec2(-1.0,1.0); vec2 offset7=vec2(0.0,1.0); vec2 offset8=vec2(1.0,1.0);
float kernelValue0 = 0.999900; float kernelValue1 = 0.999950; float kernelValue2 = 0.999900;
float kernelValue3 = 0.999950; float kernelValue4 =1.000000; float kernelValue5 = 0.999950;
float kernelValue6 = 0.999900; float kernelValue7 = 0.999950; float kernelValue8 = 0.999900;
vec4 cTemp0;vec4 cTemp1;vec4 cTemp2;vec4 cTemp3;vec4 cTemp4;vec4 cTemp5;vec4 cTemp6;vec4 cTemp7;vec4 cTemp8;
vec2 offset_coor;
offset_coor=(offset0/imageSize)*level;
cTemp0=texture2D(sTexture, vTextureCoord+offset_coor);
offset_coor=(offset1/imageSize)*level;
cTemp1=texture2D(sTexture, vTextureCoord+offset_coor);
offset_coor=(offset2/imageSize)*level;
cTemp2=texture2D(sTexture, vTextureCoord+offset_coor);
offset_coor=(offset3/imageSize)*level;
cTemp3=texture2D(sTexture, vTextureCoord+offset_coor);
offset_coor=(offset4/imageSize)*level;
cTemp4=texture2D(sTexture, vTextureCoord+offset_coor);
offset_coor=(offset5/imageSize)*level;
cTemp5=texture2D(sTexture, vTextureCoord+offset_coor);
offset_coor=(offset6/imageSize)*level;
cTemp6=texture2D(sTexture, vTextureCoord+offset_coor);
offset_coor=(offset7/imageSize)*level;
cTemp7=texture2D(sTexture, vTextureCoord+offset_coor);
offset_coor=(offset8/imageSize)*level;
cTemp8=texture2D(sTexture, vTextureCoord+offset_coor);
sum =kernelValue0*cTemp0+kernelValue1*cTemp1+kernelValue2*cTemp2+
    kernelValue3*cTemp3+kernelValue4*cTemp4+kernelValue5*cTemp5+
    kernelValue6*cTemp6+kernelValue7*cTemp7+kernelValue8*cTemp8; 
float factor=kernelValue0+kernelValue1+kernelValue2+kernelValue3+kernelValue4+kernelValue5+kernelValue6+kernelValue7+kernelValue8;
gl_FragColor = sum/factor;
}

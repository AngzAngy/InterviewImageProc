"precision mediump float;"
    "varying vec2 vTextureCoord;"
    "uniform sampler2D sTexture;"
    "uniform vec2 imageSize;"
    "uniform float level;"
    "void main( void ){"
      "float sigma_pixel = 1.5*9.0/255.0;"
      "float weight_dis[9]; float weight_pixel[9];"
      "vec4 sum_value = vec4(0.0,0.0,0.0,0.0);"
      "float sum_weight = 0.0;"
      "vec4 sample[9];"
      "float pixW = (1.0/imageSize.x)*level;"
      "float pixH = (1.0/imageSize.y)*level;"
      "sample[0] = texture2D(sTexture, vTextureCoord + vec2(-pixW, -pixH));"
      "sample[1] = texture2D(sTexture, vTextureCoord + vec2(0.0, -pixH)); "
      "sample[2] = texture2D(sTexture, vTextureCoord + vec2(pixW, -pixH));"
      "sample[3] = texture2D(sTexture, vTextureCoord + vec2(-pixW, 0.0)); "
      "sample[4] = texture2D(sTexture, vTextureCoord);                    "
      "sample[5] = texture2D(sTexture, vTextureCoord + vec2(pixW, 0.0));  "
      "sample[6] = texture2D(sTexture, vTextureCoord + vec2(-pixW, pixH));"
      "sample[7] = texture2D(sTexture, vTextureCoord + vec2(0.0, pixH));  "
      "sample[8] = texture2D(sTexture, vTextureCoord + vec2(pixW, pixH)); "
      "weight_dis[0] = 0.9984;"
      "weight_dis[1] = 0.996; "
      "weight_dis[2] = 0.9968;"
      "weight_dis[3] = 0.9992;"
      "weight_dis[4] = 1.0;   "
      "weight_dis[5] = 0.9992;"
      "weight_dis[6] = 0.9968;"
      "weight_dis[7] = 0.996; "
      "weight_dis[8] = 0.9984;"
      "for(int i=0; i<9; i++){"
      "     float dr = (sample[i].r - sample[4].r);"
      "     float dg = (sample[i].g - sample[4].g);"
      "     float db = (sample[i].b - sample[4].b);"
      "     weight_pixel[i] = exp(-(dr*dr + dg*dg + db*db)/(2.0*sigma_pixel*sigma_pixel));"
      "}"
      "for(int i=0; i<9; i++){"
      "    float temp = weight_dis[i]*weight_pixel[i];"
      "    sum_value += temp*sample[i];"
      "    sum_weight += temp;"
      "}"
      "gl_FragColor = sum_value / sum_weight;}"
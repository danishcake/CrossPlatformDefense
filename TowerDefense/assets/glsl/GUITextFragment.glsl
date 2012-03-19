varying lowp vec2 VertexTextureCoord;
uniform lowp vec4 TextColor;
uniform sampler2D texture;

void main(void) {
    lowp vec4 tx = texture2D(texture, VertexTextureCoord);
    lowp vec4 col = vec4(tx.r * TextColor.r, tx.r * TextColor.g, tx.r * TextColor.b, tx.r * TextColor.a);
    gl_FragColor = col;
}
#include "include/types.h"

Mat2f Mat2f::operator*(const Mat2f& other) const{
    return {data[0]*other.data[0] + data[1]*other.data[2],
            data[0]*other.data[1] + data[1]*other.data[3],
            data[2]*other.data[0] + data[3]*other.data[2],
            data[2]*other.data[1] + data[3]*other.data[3],};
}



V2 Mat2f::operator*(const V2& other) const{
    return {data[0]*other.x + data[1]*other.y,
            data[2]*other.x + data[2]*other.y};
}


Mat2f rotationMatrix2f(float angle){
    float s = sinf(angle);
    float c = cosf(angle);
    return {c,-s,s,c};
}




Mat3f Mat3f::operator*(const Mat3f& other) const{
        return {data[0]*other.data[0]+data[1]*other.data[3]+data[2]*other.data[6],
                data[0]*other.data[1]+data[1]*other.data[4]+data[2]*other.data[7],
                data[0]*other.data[2]+data[1]*other.data[5]+data[2]*other.data[8],
                data[3]*other.data[0]+data[4]*other.data[3]+data[5]*other.data[6],
                data[3]*other.data[1]+data[4]*other.data[4]+data[5]*other.data[7],
                data[3]*other.data[2]+data[4]*other.data[5]+data[5]*other.data[8],
                data[6]*other.data[0]+data[7]*other.data[3]+data[8]*other.data[6],
                data[6]*other.data[1]+data[7]*other.data[4]+data[8]*other.data[7],
                data[6]*other.data[2]+data[7]*other.data[5]+data[8]*other.data[8]};
    }

V3 Mat3f::operator*(const V3& other) const{
    return {data[0]*other.x+data[1]*other.y+data[2]*other.z,
            data[3]*other.x+data[4]*other.y+data[5]*other.z,
            data[6]*other.x+data[7]*other.y+data[8]*other.z};
}



Mat3f translationMatrix(V2 v2){
    return {1,0,v2.x,
            0,1,v2.y,
            0,0,1};
}

Mat3f rotationMatrix3f(float angle){
    float s = sinf(angle);
    float c = cosf(angle);
    return {c, -s, 0,
            s, c, 0,
            0, 0, 1};
}

Mat3f rotationAndTranslationMatrix(V2 v2, float angle){
    float s = sinf(angle);
    float c = cosf(angle);
    return {c, -s, v2.x,
            s, c, v2.y,
            0, 0, 1};
}

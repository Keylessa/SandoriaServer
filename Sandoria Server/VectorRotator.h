#ifndef VECTOR_ROTATOR_H
#define VECTOR_ROTATOR_H

struct FVector {
    double X;
    double Y;
    double Z;

    FVector() : X(0.0), Y(0.0), Z(0.0) {}
    FVector(double x, double y, double z) : X(x), Y(y), Z(z) {}
};

struct FRotator {
    double Pitch;
    double Yaw;
    double Roll;

    FRotator() : Pitch(0.0), Yaw(0.0), Roll(0.0) {}
    FRotator(double pitch, double yaw, double roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}
};

#endif // VECTOR_ROTATOR_H
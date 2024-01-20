#include <gtest/gtest.h>
#include "../headers/common.h"
#include "../headers/ray.h"
#include "../material/material.h"
#include "../headers/color.h"
#include "../headers/aabb.h"

TEST(CommonTest, DegreesToRadians) {
  // Test with 0 degrees
  double result = degrees_to_radians(0.0);
  EXPECT_NEAR(result, 0.0, 0.00001);

  // Test with positive degrees
  result = degrees_to_radians(45.0);
  EXPECT_NEAR(result, 0.78539816, 0.00001);

  // Test with negative degrees
  result = degrees_to_radians(-90.0);
  EXPECT_NEAR(result, -1.5707963, 0.00001);

  // Test with large degrees
  result = degrees_to_radians(180.0);
  EXPECT_NEAR(result, 3.1415926, 0.00001);
}


TEST(Vec3Test, OperatorNegation) {
  vec3 v(1.0, 2.0, 3.0);
  vec3 neg_v = -v;
  EXPECT_EQ(neg_v.x(), -1.0);
  EXPECT_EQ(neg_v.y(), -2.0);
  EXPECT_EQ(neg_v.z(), -3.0);
}

TEST(Vec3Test, OperatorSubscript) {
  vec3 v(1.0, 2.0, 3.0);
  EXPECT_EQ(v[0], 1.0);
  EXPECT_EQ(v[1], 2.0);
  EXPECT_EQ(v[2], 3.0);
}

TEST(Vec3Test, OperatorSubscriptAssignment) {
  vec3 v(1.0, 2.0, 3.0);
  v[0] = 4.0;
  v[1] = 5.0;
  v[2] = 6.0;
  EXPECT_EQ(v.x(), 4.0);
  EXPECT_EQ(v.y(), 5.0);
  EXPECT_EQ(v.z(), 6.0);
}

TEST(Vec3Test, OperatorPlusEqual) {
  vec3 v1(1.0, 2.0, 3.0);
  vec3 v2(4.0, 5.0, 6.0);
  v1 += v2;
  EXPECT_EQ(v1.x(), 5.0);
  EXPECT_EQ(v1.y(), 7.0);
  EXPECT_EQ(v1.z(), 9.0);
}

TEST(Vec3Test, OperatorMultiplyEqual) {
  vec3 v(1.0, 2.0, 3.0);
  double t = 2.0;
  v *= t;
  EXPECT_EQ(v.x(), 2.0);
  EXPECT_EQ(v.y(), 4.0);
  EXPECT_EQ(v.z(), 6.0);
}

TEST(Vec3Test, OperatorDivideEqual) {
  vec3 v(2.0, 4.0, 6.0);
  double t = 2.0;
  v /= t;
  EXPECT_EQ(v.x(), 1.0);
  EXPECT_EQ(v.y(), 2.0);
  EXPECT_EQ(v.z(), 3.0);
}

TEST(Vec3Test, Length) {
  vec3 v(3.0, 4.0, 0.0);
  EXPECT_DOUBLE_EQ(v.length(), 5.0);
}

TEST(Vec3Test, LengthSquared) {
  vec3 v(3.0, 4.0, 0.0);
  EXPECT_DOUBLE_EQ(v.length_squared(), 25.0);
}

TEST(Vec3Test, NearZeroTest) {
  // Test case 1: Vector is not near zero
  vec3 vec1(1.0, 2.0, 3.0);
  EXPECT_FALSE(vec1.near_zero());

  // Test case 2: Vector is near zero
  vec3 vec2(0.000000001, 0.000000002, 0.000000003);
  EXPECT_TRUE(vec2.near_zero());

  // Test case 3: Vector is exactly zero
  vec3 vec3(0.0, 0.0, 0.0);
  EXPECT_TRUE(vec3.near_zero());
}

TEST(Vec3Test, DotProduct) {
  vec3 u(1.0, 2.0, 3.0);
  vec3 v(4.0, 5.0, 6.0);

  double result = dot(u, v);

  // Check if the dot product is calculated correctly
  EXPECT_EQ(result, 32.0);
}

TEST(Vec3Test, OrthogonalVectors) {
  vec3 u(1.0, 0.0, 0.0);
  vec3 v(0.0, 1.0, 0.0);

  double result = dot(u, v);

  // Check if the dot product of orthogonal vectors is zero
  EXPECT_EQ(result, 0.0);
}

TEST(Vec3Test, CrossProduct) {
  // Test case 1: Cross product of (1, 0, 0) and (0, 1, 0)
  vec3 u(1, 0, 0);
  vec3 v(0, 1, 0);
  vec3 result = cross(u, v);
  vec3 expected_result(0, 0, 1);
  EXPECT_DOUBLE_EQ(result.x(), expected_result.x());
  EXPECT_DOUBLE_EQ(result.y(), expected_result.y());
  EXPECT_DOUBLE_EQ(result.z(), expected_result.z());

  // Test case 2: Cross product of (2, 3, 4) and (5, 6, 7)
  u = vec3(2, 3, 4);
  v = vec3(5, 6, 7);
  result = cross(u, v);
  expected_result = vec3(-3, 6, -3);
  EXPECT_DOUBLE_EQ(result.x(), expected_result.x());
  EXPECT_DOUBLE_EQ(result.y(), expected_result.y());
  EXPECT_DOUBLE_EQ(result.z(), expected_result.z());
}

TEST(Vec3Test, ReflectionInYAxis) {
  vec3 v(1.0, 1.0, 0.0);
  vec3 n(0.0, 1.0, 0.0);
  vec3 reflected = reflect(v, n);
  vec3 expected(1.0, -1.0, 0.0);

  EXPECT_DOUBLE_EQ(reflected.x(), expected.x());
  EXPECT_DOUBLE_EQ(reflected.y(), expected.y());
  EXPECT_DOUBLE_EQ(reflected.z(), expected.z());
}

TEST(IntervalTest, Contains) {
  interval i(0, 10);
  
  // Test if the interval contains a value within its range
  EXPECT_TRUE(i.contains(5));
  
  // Test if the interval does not contain a value outside its range
  EXPECT_FALSE(i.contains(-5));
  EXPECT_FALSE(i.contains(15));
}

TEST(IntervalTest, Surrounds) {
  interval i(0, 10);
  
  // Test if the interval surrounds a value within its range
  EXPECT_TRUE(i.surrounds(5));
  
  // Test if the interval does not surround a value outside its range
  EXPECT_FALSE(i.surrounds(0));
  EXPECT_FALSE(i.surrounds(10));
}

TEST(IntervalTest, Clamp) {
  interval i(0, 10);
  
  // Test if the interval clamps a value within its range
  EXPECT_EQ(i.clamp(5), 5);
  
  // Test if the interval clamps a value below its range
  EXPECT_EQ(i.clamp(-5), 0);
  
  // Test if the interval clamps a value above its range
  EXPECT_EQ(i.clamp(15), 10);
}

TEST(IntervalTest, Size) {
  interval i(0.0, 10.0);
  
  // Test if the interval size is calculated correctly
  EXPECT_EQ(i.size(), 10.0);
}

TEST(IntervalTest, Expand) {
  interval i(0, 10);
  
  // Test if the interval is expanded correctly
  interval expanded = i.expand(2);
  EXPECT_EQ(expanded.min, -1);
  EXPECT_EQ(expanded.max, 11);
}


TEST(RayTest, AtFunctionReturnsCorrectPoint) {
  // Create a ray with origin at (1, 2, 3) and direction (4, 5, 6)
  ray r(point3(1, 2, 3), vec3(4, 5, 6));

  // Test the at() function for different values of t
  EXPECT_EQ(r.at(0).x(), 1);
  EXPECT_EQ(r.at(0).y(), 2);
  EXPECT_EQ(r.at(0).z(), 3);

  EXPECT_EQ(r.at(1).x(), 5);
  EXPECT_EQ(r.at(1).y(), 7);
  EXPECT_EQ(r.at(1).z(), 9);

  EXPECT_EQ(r.at(-1).x(), -3);
  EXPECT_EQ(r.at(-1).y(), -3);
  EXPECT_EQ(r.at(-1).z(), -3);

  EXPECT_EQ(r.at(2.5).x(), 11);
  EXPECT_EQ(r.at(2.5).y(), 14.5);
  EXPECT_EQ(r.at(2.5).z(), 18);
}

TEST(RayTest, OriginAndDirectionFunctionsReturnCorrectValues) {
  // Create a ray with origin at (1, 2, 3) and direction (4, 5, 6)
  ray r(point3(1, 2, 3), vec3(4, 5, 6));

  // Test the origin() and direction() functions
  EXPECT_EQ(r.origin().x(), 1);
  EXPECT_EQ(r.origin().y(), 2);
  EXPECT_EQ(r.origin().z(), 3);

  EXPECT_EQ(r.direction().x(), 4);
  EXPECT_EQ(r.direction().y(), 5);
  EXPECT_EQ(r.direction().z(), 6);
}


TEST(MaterialTest, LambertianScatter) {
    lambertian mat(color(0.5, 0.5, 0.5));
    ray r_in(point3(0, 0, 0), vec3(1, 1, 1));
    hit_record rec;
    color attenuation;
    ray scattered;

    bool result = mat.scatter(r_in, rec, attenuation, scattered);

    // Assert the result and check the values of attenuation and scattered
    EXPECT_TRUE(result);
    EXPECT_EQ(attenuation.x(), 0.5);
    EXPECT_EQ(attenuation.y(), 0.5);
    EXPECT_EQ(attenuation.z(), 0.5);
    EXPECT_EQ(scattered.origin().x(), rec.p.x());
    EXPECT_EQ(scattered.origin().y(), rec.p.y());
    EXPECT_EQ(scattered.origin().z(), rec.p.z());
    // Add more assertions for scattered direction and other properties
}

TEST(MaterialTest, MetalScatter) {
    // Create a metal material with albedo (0.8, 0.8, 0.8) and fuzz 0.2
    metal mat(color(0.8, 0.8, 0.8), 0.2);

    // Create a ray pointing in the positive x direction
    ray r_in(point3(0, 0, 0), vec3(1, 0, 0));

    // Create a hit record indicating a hit at (1, 0, 0) with a normal pointing in the positive y direction
    hit_record rec;
    rec.p = point3(1, 0, 0);
    rec.normal = vec3(0, 1, 0);

    color attenuation;
    ray scattered;

    // Call the scatter function
    bool result = mat.scatter(r_in, rec, attenuation, scattered);

    // Check the result
    EXPECT_TRUE(result);

    // Check the attenuation
    EXPECT_EQ(attenuation.x(), 0.8);
    EXPECT_EQ(attenuation.y(), 0.8);
    EXPECT_EQ(attenuation.z(), 0.8);

    // Check the origin of the scattered ray
    EXPECT_EQ(scattered.origin().x(), rec.p.x());
    EXPECT_EQ(scattered.origin().y(), rec.p.y());
    EXPECT_EQ(scattered.origin().z(), rec.p.z());

    // Check the direction of the scattered ray
    // The exact direction will depend on the random unit vector, so we can't check it directly
    // But we can check that it's not equal to the original direction
    EXPECT_NE(scattered.direction().x(), r_in.direction().x());
    EXPECT_NE(scattered.direction().y(), r_in.direction().y());
    EXPECT_NE(scattered.direction().z(), r_in.direction().z());

    // Check that the dot product of the scattered direction and the normal is positive
    EXPECT_GT(dot(scattered.direction(), rec.normal), 0);
}

TEST(MaterialTest, DielectricScatter) {
    // Create a dielectric material with index of refraction 1.5
    dielectric mat(1.5);

    // Create a ray pointing in the positive x direction
    ray r_in(point3(0, 0, 0), vec3(1, 0, 0));

    // Create a hit record indicating a hit at (1, 0, 0) with a normal pointing in the positive y direction
    hit_record rec;
    rec.p = point3(1, 0, 0);
    rec.normal = vec3(0, 1, 0);
    rec.front_face = true; // Assuming the ray hits the front face of the material

    color attenuation;
    ray scattered;

    // Call the scatter function
    bool result = mat.scatter(r_in, rec, attenuation, scattered);

    // Check the result
    EXPECT_TRUE(result);

    // Check the attenuation
    EXPECT_EQ(attenuation.x(), 1.0);
    EXPECT_EQ(attenuation.y(), 1.0);
    EXPECT_EQ(attenuation.z(), 1.0);

    // Check the origin of the scattered ray
    EXPECT_EQ(scattered.origin().x(), rec.p.x());
    EXPECT_EQ(scattered.origin().y(), rec.p.y());
    EXPECT_EQ(scattered.origin().z(), rec.p.z());
}

TEST(MaterialTest, DiffuseLightScatter) {
    diffuse_light mat(color(1.0, 1.0, 1.0));
    ray r_in(point3(0, 0, 0), vec3(1, 1, 1));
    hit_record rec;
    color attenuation;
    ray scattered;

    bool result = mat.scatter(r_in, rec, attenuation, scattered);

    // Assert the result and check the values of attenuation and scattered
    EXPECT_FALSE(result);
    EXPECT_EQ(scattered.origin().x(), rec.p.x());
    EXPECT_EQ(scattered.origin().y(), rec.p.y());
    EXPECT_EQ(scattered.origin().z(), rec.p.z());
    // Add more assertions for emitted color and other properties
}

TEST(MaterialTest, DiffuseLightEmitted) {
    diffuse_light mat(color(1.0, 1.0, 1.0));
    double u = 0.5;
    double v = 0.5;
    point3 p(1, 1, 1);

    color emittedColor = mat.emitted(u, v, p);

    // Assert the emitted color value
    EXPECT_EQ(emittedColor.x(), 1.0);
    EXPECT_EQ(emittedColor.y(), 1.0);
    EXPECT_EQ(emittedColor.z(), 1.0);
    // Add more assertions for emitted color properties
}

TEST(MaterialTest, DiffuseLight) {
    // Create a diffuse light with a solid color texture
    color c(0.8, 0.8, 0.8);
    auto texture = make_shared<solid_color>(c);
    diffuse_light mat(texture);

    // Test the scatter method
    // Create a ray pointing in the positive x direction
    ray r_in(point3(0, 0, 0), vec3(1, 0, 0));

    // Create a hit record indicating a hit at (1, 0, 0) with a normal pointing in the positive y direction
    hit_record rec;
    rec.p = point3(1, 0, 0);
    rec.normal = vec3(0, 1, 0);

    color attenuation;
    ray scattered;

    // Call the scatter function
    bool result = mat.scatter(r_in, rec, attenuation, scattered);

    // Check the result
    EXPECT_FALSE(result);

    // Test the emitted method
    color emitted = mat.emitted(0.5, 0.5, point3(1, 0, 0));

    // Check the emitted color
    EXPECT_EQ(emitted.x(), c.x());
    EXPECT_EQ(emitted.y(), c.y());
    EXPECT_EQ(emitted.z(), c.z());
}

TEST(AABBTest, Constructor) {
  // Test the default constructor
  aabb box1;
  EXPECT_EQ(box1.x.min, std::numeric_limits<double>::infinity());
  EXPECT_EQ(box1.x.max, -std::numeric_limits<double>::infinity());
  EXPECT_EQ(box1.y.min, std::numeric_limits<double>::infinity());
  EXPECT_EQ(box1.y.max, -std::numeric_limits<double>::infinity());
  EXPECT_EQ(box1.z.min, std::numeric_limits<double>::infinity());
  EXPECT_EQ(box1.z.max, -std::numeric_limits<double>::infinity());

  // Test the constructor with intervals
  interval ix(1.0, 2.0);
  interval iy(3.0, 4.0);
  interval iz(5.0, 6.0);
  aabb box2(ix, iy, iz);
  EXPECT_EQ(box2.x.min, 1.0);
  EXPECT_EQ(box2.x.max, 2.0);
  EXPECT_EQ(box2.y.min, 3.0);
  EXPECT_EQ(box2.y.max, 4.0);
  EXPECT_EQ(box2.z.min, 5.0);
  EXPECT_EQ(box2.z.max, 6.0);

  // Test the constructor with points
  point3 a(1.0, 2.0, 3.0);
  point3 b(4.0, 5.0, 6.0);
  aabb box3(a, b);
  EXPECT_EQ(box3.x.min, 1.0);
  EXPECT_EQ(box3.x.max, 4.0);
  EXPECT_EQ(box3.y.min, 2.0);
  EXPECT_EQ(box3.y.max, 5.0);
  EXPECT_EQ(box3.z.min, 3.0);
  EXPECT_EQ(box3.z.max, 6.0);

  // Test the constructor with two boxes
  aabb box4(box2, box3);
  EXPECT_EQ(box4.x.min, 1.0);
  EXPECT_EQ(box4.x.max, 4.0);
  EXPECT_EQ(box4.y.min, 2.0);
  EXPECT_EQ(box4.y.max, 5.0);
  EXPECT_EQ(box4.z.min, 3.0);
  EXPECT_EQ(box4.z.max, 6.0);
}

TEST(AABBTest, Pad) {
  // Test the pad method
  aabb box(interval(1.0, 2.0), interval(3.0, 4.0), interval(5.0, 6.0));
  aabb paddedBox = box.pad();
  EXPECT_NEAR(paddedBox.x.min, 0.9999, 0.0002);
  EXPECT_NEAR(paddedBox.x.max, 2.0001, 0.0002);
  EXPECT_NEAR(paddedBox.y.min, 2.9999, 0.0002);
  EXPECT_NEAR(paddedBox.y.max, 4.0001, 0.0002);
  EXPECT_NEAR(paddedBox.z.min, 4.9999, 0.0002);
  EXPECT_NEAR(paddedBox.z.max, 6.0001, 0.0002);
}

TEST(AABBTest, Axis) {
  // Test the axis method
  aabb box(interval(1.0, 2.0), interval(3.0, 4.0), interval(5.0, 6.0));
  EXPECT_EQ(box.axis(0).min, 1.0);
  EXPECT_EQ(box.axis(0).max, 2.0);
  EXPECT_EQ(box.axis(1).min, 3.0);
  EXPECT_EQ(box.axis(1).max, 4.0);
  EXPECT_EQ(box.axis(2).min, 5.0);
  EXPECT_EQ(box.axis(2).max, 6.0);
}


#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"


#include <iostream>


/*

To run:
cmd
cd C:\Users\gfray\Documents\Visual Studio 2022\Projects\RayTracingInOneWeekend\RayTracingIn1\RayTracing1\x64\Debug
RayTracer1.exe > image.ppm

*/

double hit_sphere(const point3& center, double radius, const ray& r)
{
	vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();// dot(r.direction(), r.direction());
	//auto b = 2.0 * dot(oc, r.direction());
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;// dot(oc, oc) - radius * radius;
	auto discriminant = half_b * half_b - (a * c);// b* b - (4 * a * c);
	if (discriminant < 0)
	{
		return -1.0;
	}
	else
	{
		return (-half_b - sqrt(discriminant)) / (a);
	}
}

color ray_color(const ray& r, const hittable& world, int depth)
{
	hit_record rec;

	if (depth <= 0)
	{
		return color(0, 0, 0);
	}

	if (world.hit(r, 0.001, infinity, rec))
	{
		//method 3
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * ray_color(scattered, world, depth - 1);
		}
		return color(0, 0, 0);

		 //method 2
		////point3 target = rec.p + random_in_hemisphere(rec.normal);
		////return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
		
		//return 0.5 * (rec.normal + color(1, 1, 1));
	}

	/*auto t = hit_sphere(point3(0,0,-1), 0.5, r);
	if (t > 0.0)
	{
		vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
		return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
	}*/
	/*if (hit_sphere(point3(0, 0, -1), 0.5, r))
	{
		return color (1,0,1);
	}*/
	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);//((1.0 - t) * color(1.0, 1.0, 1.0)) + (t * color(0.5, 0.7, 1.0));
}


int main()
{
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;// 256;
	const int image_height = static_cast<int>(image_width / aspect_ratio);// 256;
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	hittable_list world;
	////world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, NULL));
	////world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, NULL));

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	/*auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);*/
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(1.5);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	camera cam;
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = point3(0, 0, 0);
	auto horizontal = vec3(viewport_width, 0, 0);
	auto vertical = vec3(0, viewport_height, 0);
	auto lower_left_corner = origin - (horizontal / 2) - (vertical / 2) - vec3(0, 0, focal_length);


	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		//std::cerr << "|";//// << j << ' ' << std::flush;

		for (int i = 0; i < image_width; i++)
		{
			//v3, with camera now
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s)
			{
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);

				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}
			write_color(std::cout, pixel_color, samples_per_pixel);


			/* obsolete v2
			auto u = double(i) / (image_width - 1);
			auto v = double(j) / (image_height - 1);
			ray r(origin, lower_left_corner + (u * horizontal) + (v * vertical) - origin);
			color pixel_color = ray_color(r, world);

			write_color(std::cout, pixel_color);
			*/

			//color pixel_color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
			//write_color(std::cout, pixel_color);

			/*auto r = double(i) / (image_width - 1);
			auto g = double(j) / (image_height - 1);
			auto b = 0.25;

			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);

			std::cout << ir << ' ' << ig << ' ' << ib << '\n';*/
		}
	}


	return 0;
}
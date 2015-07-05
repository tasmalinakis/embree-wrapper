#pragma once
#include <glm\glm.hpp>
#include <string>

namespace path_tracer
{
	class Material
	{

	public:

		Material(size_t mat_id, const std::string mat_name) : mat_id(mat_id), mat_name(mat_name) {}
		~Material() {}

		glm::vec3 sample();

		// brdf getters
		inline float getSpecularExponent() { return brdf.Ns; }
		inline float getRefractionIndex() { return brdf.Ni; }
		inline glm::dvec3 getReflectivityAmbient() { return brdf.Ka; }
		inline glm::dvec3 getReflectivityDiffuse() { return brdf.Kd; }
		inline glm::dvec3 getReflectivitySpecular() { return brdf.Ks; }
		inline glm::dvec3 getTransmissionFilter() { return brdf.Kt; }
		inline glm::dvec3 getRadiance() { return radiance; }

		// brdf setters
		inline void setSpecularExponent(float Ns) { brdf.Ns = Ns; }
		inline void setRefractionIndex(float Ni) { brdf.Ni = Ni; }
		inline void setReflectivityAmbient(glm::dvec3 Ka) { brdf.Ka = Ka; }
		inline void setReflectivityDiffuse(glm::dvec3 Kd) { brdf.Kd = Kd; }
		inline void setReflectivitySpecular(glm::dvec3 Ks) { brdf.Ks = Ks; }
		inline void setTransmissionFilter(glm::dvec3 Kt) { brdf.Kt = Kt; }
		inline void setRadiance(glm::dvec3 radiance) { this->radiance = radiance; }

		inline const std::string getName() { return mat_name; }

	private:
		
		const size_t mat_id;
		const std::string mat_name;

		glm::dvec3 radiance;

		// the brdf of the material
		struct BRDF
		{
			float Ns;               /*< specular exponent */
			float Ni;               /*< optical density for the surface (index of refraction) */
			glm::dvec3 Ka;              /*< ambient reflectivity */
			glm::dvec3 Kd;              /*< diffuse reflectivity */
			glm::dvec3 Ks;              /*< specular reflectivity */
			glm::dvec3 Kt;              /*< transmission filter */
		} brdf;
	};
}
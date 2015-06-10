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
		inline glm::vec3 getReflectivityAmbient() { return brdf.Ka; }
		inline glm::vec3 getReflectivityDiffuse() { return brdf.Kd; }
		inline glm::vec3 getReflectivitySpecular() { return brdf.Ks; }
		inline glm::vec3 getTransmissionFilter() { return brdf.Kt; }

		// brdf setters
		inline Material* setSpecularExponent(float Ns) { brdf.Ns = Ns; return this; }
		inline Material* setRefractionIndex(float Ni) { brdf.Ni = Ni; return this; }
		inline Material* setReflectivityAmbient(glm::vec3 Ka) { brdf.Ka = Ka; return this; }
		inline Material* setReflectivityDiffuse(glm::vec3 Kd) { brdf.Kd = Kd; return this; }
		inline Material* setReflectivitySpecular(glm::vec3 Ks) { brdf.Ks = Ks; return this; }
		inline Material* setTransmissionFilter(glm::vec3 Kt) { brdf.Kt = Kt; return this; }

		inline const std::string getName() { return mat_name; }

	private:
		
		const size_t mat_id;
		const std::string mat_name;

		// the brdf of the material
		struct BRDF
		{
			float Ns;               /*< specular exponent */
			float Ni;               /*< optical density for the surface (index of refraction) */
			glm::vec3 Ka;              /*< ambient reflectivity */
			glm::vec3 Kd;              /*< diffuse reflectivity */
			glm::vec3 Ks;              /*< specular reflectivity */
			glm::vec3 Kt;              /*< transmission filter */
		} brdf;
	};
}
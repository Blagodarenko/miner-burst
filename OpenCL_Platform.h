#include <string>
#include <vector>
#include <exception>
#include <memory>
#include <CL/cl.h>

#ifndef OPENCL_PLATFORM_H
#define OPENCL_PLATFORM_H



class OpenclPlatform {
	private:
		cl_platform_id m_handle;

	public:
			OpenclPlatform(const cl_platform_id& p_handle);
			OpenclPlatform(const OpenclPlatform& p_other);
			virtual ~OpenclPlatform();

			inline const cl_platform_id& getHandle() const;
			
			OpenclPlatform& operator=(const OpenclPlatform& p_other);

			std::string getName() const throw (std::exception);
			std::string getVendor() const throw (std::exception);
			std::string getVersion() const throw (std::exception);
			
		private:
			std::string getInfoString(const cl_platform_info& p_paramName) const throw (std::exception);

		public:
			static std::vector<std::shared_ptr<OpenclPlatform>> list() throw (std::exception);
		};


		inline const cl_platform_id& OpenclPlatform::getHandle() const {
			return m_handle;
		}
#endif		
#pragma once

#include <string>
#include <filesystem>
#include <boost/iostreams/device/mapped_file.hpp>
#include <optional>
#include <thread>

namespace fileutils
{

    class File
    {
    public:
        File(
            const std::string_view file_path
        ) : file_path(file_path)
        {}


        ~File()
        {}

        virtual size_t GetSize() const
        {
            if (!IsFileExist(file_path)) return 0;

            return std::filesystem::file_size(file_path);
        }

        bool Truncate(size_t new_size)                                         
         {                                                                      
             if (!IsExist()) {                                                  
                 return false;                                                  
             }                                                                  
                                                                                
             try {                                                              
                 // Resize the file using filesystem                            
                 std::filesystem::resize_file(file_path, new_size);             
                 return true;                                                   
             } catch (const std::exception&) {                                  
                 return false;                                                  
             }                                                                  
         }

        size_t CreateBinary(
            uint8_t         default_value,
            size_t          size,
            bool            overwrite = false
        )
        {
            if (IsFileExist(file_path)
                && !overwrite)
            {
                return 0;
            }

            boost::iostreams::mapped_file_params params {};

            params.path = file_path;
            params.new_file_size = size;
            params.flags = boost::iostreams::mapped_file::mapmode::readwrite;

            boost::iostreams::mapped_file_sink file(params);

            auto sz = file.size();
            std::memset(file.data(), default_value, file.size());

            sink.close();
            return size;
        }

        bool IsExist() const noexcept
        {
            return IsFileExist(this->file_path);
        }

        const std::string file_path;

        boost::iostreams::mapped_file_sink sink;

    protected:

        bool Open()
        {
            auto params = GetIOParams();
            sink.open(params);

            return true;
        }

        void Close()
        {
            sink.close();
        }

        boost::iostreams::mapped_file_params GetIOParams() const
        {
            boost::iostreams::mapped_file_params params;
            params.path = file_path;
            params.new_file_size = 0;
            params.flags = boost::iostreams::mapped_file::mapmode::readwrite;

            return params;
        }

    private:
        inline bool IsFileExist(
            std::string_view file_path
        ) const
        {
            return std::filesystem::exists(file_path);
        }
    };

};
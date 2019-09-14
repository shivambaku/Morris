import os
import shutil

source_path = 'morris/src/'
source_build_path = 'morris/build/'
destination_inc_path = 'morris_ui/cpp_addons/inc/morris/'
destination_lib_path = 'morris_ui/cpp_addons/lib/'

# check if the source build directory exist
if not os.path.isdir(source_build_path):
    print('No build directory! Please build the project first.')
    quit()

# different os have different settings
if os.name == 'nt':
    source_build_path = os.path.join(source_build_path, 'release')
    lib_extension = '.lib'
else:
    lib_extension = '.a'

# remove any existing includes if they exist
if os.path.exists(destination_inc_path):
    shutil.rmtree(destination_inc_path)

# ignore all files but header (.hpp) files
ignore_func = lambda d, files: [f for f in files if os.path.isfile(os.path.join(d, f)) and not f.endswith('.hpp')]

# copy all folders and sub folders from source to destination
shutil.copytree(source_path, destination_inc_path, ignore=ignore_func)

# make a lib directory if it does not already exist
os.makedirs(destination_lib_path, exist_ok = True)

# find the library file and copy it to the destination lib folder
for file in os.listdir(source_build_path):
    if file.endswith(lib_extension):
        shutil.copy(os.path.join(source_build_path, file), destination_lib_path)




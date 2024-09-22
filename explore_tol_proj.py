import os
import pandas as pd
from collections import defaultdict

def get_directory_info(root_dir, max_depth=None):
    dir_info = defaultdict(lambda: {'size': 0, 'num_files': 0, 'num_dirs': 0, 'extensions': defaultdict(int)})
    
    def explore_dir(current_dir, current_depth):
        if max_depth is not None and current_depth > max_depth:
            return
        
        for dirpath, dirnames, filenames in os.walk(current_dir):
            relative_path = os.path.relpath(dirpath, root_dir)
            
            # Update folder count for the current directory
            dir_info[relative_path]['num_dirs'] += len(dirnames)
            
            for file in filenames:
                file_path = os.path.join(dirpath, file)
                file_size = os.path.getsize(file_path)
                file_extension = os.path.splitext(file)[1].lower()

                # Update file count, size, and extension count for the current directory
                dir_info[relative_path]['num_files'] += 1
                dir_info[relative_path]['size'] += file_size
                dir_info[relative_path]['extensions'][file_extension] += 1

            # Explore subdirectories if we haven't reached max depth
            for sub_dir in dirnames:
                explore_dir(os.path.join(dirpath, sub_dir), current_depth + 1)

            break  # Prevent os.walk from recursively going deeper automatically

    explore_dir(root_dir, 0)
    return dir_info

def export_sorted_data_pandas(dir_info):
    # Convert directory info to a DataFrame
    data = []
    for directory, info in dir_info.items():
        size_in_mb = info['size'] / (1024 * 1024)  # Convert size to MB
        ext_summary = ", ".join([f"{ext} ({count})" for ext, count in info['extensions'].items()])
        data.append([directory, info['num_dirs'], info['num_files'], size_in_mb, ext_summary])

    df = pd.DataFrame(data, columns=["Directory", "Folders", "Files", "Size (MB)", "Extensions"])

    # Export sorted by Folders
    df.sort_values(by="Folders", ascending=False).to_csv('sorted_by_folders.csv', index=False)

    # Export sorted by Files
    df.sort_values(by="Files", ascending=False).to_csv('sorted_by_files.csv', index=False)

    # Export sorted by Size
    df.sort_values(by="Size (MB)", ascending=False).to_csv('sorted_by_size.csv', index=False)

# Define the root directory (the folder where the repository is located) and max depth
root_directory = '.'  # Change this if you want to start elsewhere
max_depth = 1  # Set your desired depth, None for no limit

# Get directory information
directory_info = get_directory_info(root_directory, max_depth=max_depth)

# Export sorted data using pandas
export_sorted_data_pandas(directory_info)

print("CSV files have been created: sorted_by_folders.csv, sorted_by_files.csv, sorted_by_size.csv")

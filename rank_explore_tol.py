import os
import pandas as pd
import fnmatch


def get_directory_info(directory, exclude_patterns=None, use_exclude_patterns=False):
    """Get info about immediate subdirectories and files in the directory."""
    num_immediate_subdirs = 0
    num_immediate_files = 0
    total_immediate_size = 0
    immediate_extensions = {}
    try:
        with os.scandir(directory) as entries:
            for entry in entries:
                # Check for exclusion patterns
                if use_exclude_patterns and exclude_patterns:
                    excluded = False
                    for pattern in exclude_patterns:
                        if fnmatch.fnmatch(entry.name, pattern):
                            excluded = True
                            break
                    if excluded:
                        continue

                if entry.is_dir(follow_symlinks=False):
                    num_immediate_subdirs += 1
                elif entry.is_file(follow_symlinks=False):
                    num_immediate_files += 1
                    total_immediate_size += entry.stat().st_size
                    ext = os.path.splitext(entry.name)[1].lower()
                    immediate_extensions[ext] = immediate_extensions.get(ext, 0) + 1
    except PermissionError:
        # Skip directories we don't have permission to access
        pass

    return {
        'num_immediate_subdirs': num_immediate_subdirs,
        'num_immediate_files': num_immediate_files,
        'total_immediate_size': total_immediate_size,
        'immediate_extensions': immediate_extensions
    }


def explore_directory(
        directory, base_directory, explored_dirs=None,
        max_depth=None, current_depth=0, data_list=None,
        cumulative=True, exclude_patterns=None, use_exclude_patterns=False
):
    if explored_dirs is None:
        explored_dirs = set()
    if data_list is None:
        data_list = []
    directory = os.path.abspath(directory)

    # Check for exclusion patterns
    if use_exclude_patterns and exclude_patterns:
        for pattern in exclude_patterns:
            if fnmatch.fnmatch(os.path.basename(directory), pattern):
                print(f"Excluding directory: {directory}")
                return {'num_cumulative_subdirs': 0, 'num_cumulative_files': 0,
                        'total_cumulative_size': 0, 'cumulative_extensions': {}}

    if directory in explored_dirs:
        return {'num_cumulative_subdirs': 0, 'num_cumulative_files': 0,
                'total_cumulative_size': 0, 'cumulative_extensions': {}}
    if max_depth is not None and current_depth > max_depth:
        return {'num_cumulative_subdirs': 0, 'num_cumulative_files': 0,
                'total_cumulative_size': 0, 'cumulative_extensions': {}}
    explored_dirs.add(directory)
    print(f"Exploring directory: {directory}")

    info = get_directory_info(directory, exclude_patterns, use_exclude_patterns)

    # Remove base directory from the paths
    relative_dir = os.path.relpath(directory, base_directory)
    parent_dir = os.path.relpath(os.path.dirname(directory), base_directory)

    # Initialize cumulative counts with immediate counts
    num_cumulative_subdirs = info['num_immediate_subdirs']
    num_cumulative_files = info['num_immediate_files']
    total_cumulative_size = info['total_immediate_size']
    cumulative_extensions = info['immediate_extensions'].copy()

    # Proceed to explore subdirectories
    try:
        with os.scandir(directory) as entries:
            entries = list(entries)  # Convert to list to allow multiple iterations
            for entry in entries:
                # Check for exclusion patterns
                if use_exclude_patterns and exclude_patterns:
                    excluded = False
                    for pattern in exclude_patterns:
                        if fnmatch.fnmatch(entry.name, pattern):
                            excluded = True
                            break
                    if excluded:
                        continue

                if entry.is_dir(follow_symlinks=False):
                    subdir_path = os.path.join(directory, entry.name)
                    subdir_info = explore_directory(
                        subdir_path, base_directory, explored_dirs, max_depth, current_depth + 1, data_list,
                        cumulative, exclude_patterns, use_exclude_patterns)
                    if cumulative:
                        # Sum up counts from subdirectories
                        num_cumulative_subdirs += subdir_info['num_cumulative_subdirs']
                        num_cumulative_files += subdir_info['num_cumulative_files']
                        total_cumulative_size += subdir_info['total_cumulative_size']
                        # Merge extensions
                        for ext, count in subdir_info['cumulative_extensions'].items():
                            cumulative_extensions[ext] = cumulative_extensions.get(ext, 0) + count
    except PermissionError:
        # Skip directories we don't have permission to access
        pass

    # Round sizes to 3 decimals
    total_immediate_size_mb = round(info['total_immediate_size'] / (1024 * 1024), 3)
    total_cumulative_size_mb = round(total_cumulative_size / (1024 * 1024), 3)
    data_entry = {
        'Directory': relative_dir,
        'Depth': current_depth,
        'Rank_Subdirs': None,  # Placeholder for ranking
        'Rank_Files': None,
        'Rank_Size': None,
        'Average_Rank': None,
        'Immediate_Num_Subdirs': info['num_immediate_subdirs'],
        'Immediate_Num_Files': info['num_immediate_files'],
        'Immediate_Total_Size_MB': total_immediate_size_mb,
        'Cumulative_Num_Subdirs': num_cumulative_subdirs,
        'Cumulative_Num_Files': num_cumulative_files,
        'Cumulative_Total_Size_MB': total_cumulative_size_mb,
        'Immediate_Extensions': info['immediate_extensions'],
        'Cumulative_Extensions': cumulative_extensions,
        'Parent_Directory': parent_dir
    }
    data_list.append(data_entry)
    return {
        'num_cumulative_subdirs': num_cumulative_subdirs,
        'num_cumulative_files': num_cumulative_files,
        'total_cumulative_size': total_cumulative_size,
        'cumulative_extensions': cumulative_extensions
    }


def rank_directories(data_list, optimize_extensions=True):
    df = pd.DataFrame(data_list)

    # Use cumulative counts for ranking
    num_subdirs_col = 'Cumulative_Num_Subdirs'
    num_files_col = 'Cumulative_Num_Files'
    total_size_col = 'Cumulative_Total_Size_MB'

    # Compute global rankings using cumulative metrics
    df['Rank_Subdirs'] = df[num_subdirs_col].rank(ascending=False, method='min').astype(int)
    df['Rank_Files'] = df[num_files_col].rank(ascending=False, method='min').astype(int)
    df['Rank_Size'] = df[total_size_col].rank(ascending=False, method='min').astype(int)

    # Round Average_Rank to one decimal
    df['Average_Rank'] = df[['Rank_Subdirs', 'Rank_Files', 'Rank_Size']].mean(axis=1).round(1)

    # Convert specified columns to integers
    int_cols = [
        'Rank_Subdirs', 'Rank_Files', 'Rank_Size',
        'Cumulative_Num_Subdirs', 'Cumulative_Num_Files',
        'Immediate_Num_Subdirs', 'Immediate_Num_Files'
    ]
    df[int_cols] = df[int_cols].astype(int)

    # Move 'Directory' to the beginning
    columns = ['Directory', 'Depth', 'Rank_Subdirs', 'Rank_Files', 'Rank_Size', 'Average_Rank',
               'Cumulative_Num_Subdirs', 'Cumulative_Num_Files', 'Cumulative_Total_Size_MB',
               'Immediate_Num_Subdirs', 'Immediate_Num_Files', 'Immediate_Total_Size_MB']

    df = df[columns + ['Immediate_Extensions', 'Cumulative_Extensions', 'Parent_Directory']]

    # Collect all unique extensions from cumulative extensions
    all_extensions = set()
    for ext_dict in df['Cumulative_Extensions']:
        all_extensions.update(ext_dict.keys())

    all_extensions = sorted(all_extensions)

    if optimize_extensions:
        # Use optimized method to add extension columns
        # Create DataFrames for cumulative and immediate extensions
        cumulative_ext_df = pd.DataFrame(df['Cumulative_Extensions'].tolist()).fillna(0).astype(int)
        cumulative_ext_df.columns = [f'Ext_{ext}' for ext in cumulative_ext_df.columns]

        immediate_ext_df = pd.DataFrame(df['Immediate_Extensions'].tolist()).fillna(0).astype(int)
        immediate_ext_df.columns = [f'Immediate_Ext_{ext}' for ext in immediate_ext_df.columns]

        # Concatenate extension DataFrames to df
        df = pd.concat([df.reset_index(drop=True), cumulative_ext_df.reset_index(drop=True), immediate_ext_df.reset_index(drop=True)], axis=1)
    else:
        # Original method of adding extension columns
        # Create columns for each extension with cumulative counts
        for ext in all_extensions:
            df[f'Ext_{ext}'] = df['Cumulative_Extensions'].apply(lambda x: x.get(ext, 0))

        # Optionally, create columns for immediate extensions
        for ext in all_extensions:
            df[f'Immediate_Ext_{ext}'] = df['Immediate_Extensions'].apply(lambda x: x.get(ext, 0))

    # Drop the original extensions columns
    df.drop(columns=['Cumulative_Extensions', 'Immediate_Extensions', 'Parent_Directory'], inplace=True)

    # Sort the DataFrame by 'Average_Rank' in descending order at the end
    df.sort_values(by='Average_Rank', ascending=True, inplace=True)
    return df


# Start exploration from the repository path
start_directory = r'C:\Users\i_bau\OneDrive\aaaWork\repos\tol'  # Replace with your repository path
base_directory = os.path.abspath(start_directory)

# Define exclude patterns (folders and files to exclude)
exclude_patterns = ['.git', '.gitignore', '*.pyc', '__pycache__', 'env', 'venv', '*.egg-info']

# Set max_depth to None to explore all levels
data_list = []
explore_directory(start_directory, base_directory, data_list=data_list, cumulative=True,
                  exclude_patterns=exclude_patterns, use_exclude_patterns=True)

# Rank directories
# Set optimize_extensions=True to use the optimized method that avoids the PerformanceWarning
final_df = rank_directories(data_list, optimize_extensions=True)

# Existing code remains unchanged up to this point

# Rank directories
# Set optimize_extensions=True to use the optimized method that avoids the PerformanceWarning
final_df = rank_directories(data_list, optimize_extensions=True)

# **Begin of added code for rearranging columns**

# Step 1: Move 'Rank' columns to the beginning
rank_columns = [col for col in final_df.columns if 'Rank' in col]

# Step 2: Identify cumulative and immediate extension columns
cumulative_ext_columns = [col for col in final_df.columns if col.startswith('Ext_')]
immediate_ext_columns = [col for col in final_df.columns if col.startswith('Immediate_Ext_')]

# Step 3: Calculate total sums for cumulative extension columns
cumulative_ext_sums = final_df[cumulative_ext_columns].sum().sort_values(ascending=False)
cumulative_ext_columns_sorted = cumulative_ext_sums.index.tolist()

# Step 4: Calculate total sums for immediate extension columns
immediate_ext_sums = final_df[immediate_ext_columns].sum().sort_values(ascending=False)
immediate_ext_columns_sorted = immediate_ext_sums.index.tolist()

# Step 5: Maintain the order of other columns (excluding extension and rank columns)
other_columns = [col for col in final_df.columns if col not in rank_columns + cumulative_ext_columns + immediate_ext_columns]

# Step 6: Rearrange the DataFrame columns
new_column_order = rank_columns + other_columns + cumulative_ext_columns_sorted + immediate_ext_columns_sorted
final_df = final_df[new_column_order]

# **End of added code**

# Save to CSV
final_df.to_csv('exploration_results.csv', index=False)

print("Exploration completed. Results saved to 'exploration_results.csv'.")
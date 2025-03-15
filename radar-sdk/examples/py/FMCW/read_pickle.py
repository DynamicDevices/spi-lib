import pickle

def read_pickle_file(file_path):
    try:
        # Open the file in binary read mode
        with open(file_path, "rb") as f:
            # Use pickle.load to deserialize the object from the file
            obj = pickle.load(f)
            return obj
    except FileNotFoundError:
        print(f"Error: File {file_path} not found.")
        return None
    except pickle.UnpicklingError as e:
        print(f"Error: Unable to read pickle file {file_path}: {e}")
        return None

# Example usage:
file_path = "Pickle_Out/MUC/0.pkl"
obj = read_pickle_file(file_path)

print("Object read from pickle file:")
print(obj)
print(type(obj), obj.shape)
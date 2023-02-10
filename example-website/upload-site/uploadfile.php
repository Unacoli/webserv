<?php
    if (isset($_FILES['myfile']) && $_FILES['myfile']['error'] == 0) {
        $temp_file = $_FILES['myfile']['tmp_name'];
        //$file_contents = file_get_contents($temp_file);
        //echo "File contents:<br>";
        //echo "<pre>" . $file_contents . "</pre>";
        $file_name = $_FILES['myfile']['name'];
        $file_path = getcwd() . '/Downloads/' . $file_name;
        if (move_uploaded_file($temp_file, $file_path)) {
            echo "File was successfully saved to " . $file_path;
        }
    } 
    else if ($_FILES['myfile']['error'] === UPLOAD_ERR_INI_SIZE) {
        echo "The uploaded file exceeds the upload_max_filesize limit specified in the PHP configuration.";
    } else if ($_FILES['myfile']['error'] === UPLOAD_ERR_FORM_SIZE) {
        echo "The uploaded file exceeds the MAX_FILE_SIZE directive specified in the HTML form.";
    } else if ($_FILES['myfile']['error'] === UPLOAD_ERR_PARTIAL) {
        echo "The file was only partially uploaded.";
    } else if ($_FILES['myfile']['error'] === UPLOAD_ERR_NO_FILE) {
        echo "No file was uploaded.";
    } else if ($_FILES['myfile']['error'] === UPLOAD_ERR_NO_TMP_DIR) {
        echo "Missing a temporary folder.";
    } else if ($_FILES['myfile']['error'] === UPLOAD_ERR_CANT_WRITE) {
        echo "Failed to write the file to disk.";
    } else if ($_FILES['myfile']['error'] === UPLOAD_ERR_EXTENSION) {
        echo "A PHP extension stopped the file upload.";
    }    
    else {
        echo "File upload failed.";
    }

?>
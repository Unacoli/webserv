<?php

    if (isset($_FILES['myfile']) && $_FILES['myfile']['error'] == 0) {
        $temp_file = $_FILES['myfile']['tmp_name'];
        //$file_contents = file_get_contents($temp_file);
        //echo "File contents:<br>";
        //echo "<pre>" . $file_contents . "</pre>";
        $file_name = $_FILES['myfile']['name'];
        $file_path = getcwd() . '/downloads/' . $file_name;
        if (move_uploaded_file($temp_file, $file_path)) {
            echo "File was successfully saved to " . $file_path;
        }
    } else {
        echo "File upload failed.";
    }

?>
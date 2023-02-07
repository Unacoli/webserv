<?php
    //Here we gonna handle a file uploaded through POST method. The file is a multipart/form-data
    //First we gonna check if the file was uploaded
    if (isset($_FILES['uploadedfile'])) {
        //We gonna check if the file was uploaded correctly
        if ($_FILES['uploadedfile']['error'] == 0) {
                //We gonna check if the file is not too big
                if ($_FILES['uploadedfile']['size'] < 1000000) {
                        //We gonna check if the file is not already in the directory
                        if (!file_exists('uploads/' . $_FILES['uploadedfile']['name'])) {
                            //We gonna move the file to the uploads directory
                            if (move_uploaded_file($_FILES['uploadedfile']['tmp_name'], '/Downloads' . $_FILES['uploadedfile']['name'])) {
                                //We gonna display a message to the user
                                echo 'File uploaded successfully!';
                            } else {
                                //We gonna display a message to the user
                                echo 'Error uploading file - check destination is writeable.';
                            }
                        } else {
                            //We gonna display a message to the user
                            echo 'File already exists.';
                        }
                } else {
                    //We gonna display a message to the user
                    echo 'File uploaded exceeds maximum upload size.';}
            } else {
            //We gonna display a message to the user
            echo 'Error uploading file - check destination is writeable.';}
        } else {
        //We gonna display a message to the user
        echo 'No file uploaded.';
    }
?>
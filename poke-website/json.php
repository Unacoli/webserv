<?php
    /* check if there is a post request */
    if(iset($_POST)){
        /* catch incoming json data */
        $data = json_decode(file_get_contents("php://input"), true);

    }
?>
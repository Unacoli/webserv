<?php
    /* check if there is a post request */
    echo("<script>console.log('PHP: " . isset($_POST) . "');</script>");
    if(isset($_POST)){
        /* catch incoming json data */
        $data = json_decode(file_get_contents("php://input"), true);
        echo ("after data");
    }
?>
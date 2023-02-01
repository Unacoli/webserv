<?php
    // Get the data from the POST
    $data = json_decode(file_get_contents('php://input'), true);
 
    // Write the data to the JSON file
    $json_file = '/mnt/nfs/homes/barodrig/webserv/poke-website/data/data.json';
    $handle = fopen($json_file, 'w') or die('Cannot open file:  '.$json_file);
    fwrite($handle, json_encode($data));
    fclose($handle);
 
    // Generate an HTML page to display the data
    $html = '<html><body>';
    $html .= '<h1>Data received:</h1>';
    $html .= '<p>Name: '.$data['name'].'</p>';
    $html .= '<p>Age: '.$data['age'].'</p>';
    $html .= '<p>Vote: '.$data['vote'].'</p>';
    $html .= '</body></html>';
 
    // Write the HTML to a file
    echo $html;
?>
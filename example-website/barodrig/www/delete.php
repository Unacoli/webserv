<?php

if ($_SERVER['REQUEST_METHOD'] === 'DELETE') { 
   		echo "You send a DELETE REQUEST\n\n";
	$status=unlink('delete1.html');    
	if($status){  
		echo "File deleted successfully\n";    
	}else{
		echo "Sorry!\n";    
	}  
}
else
	echo "This is not a Delete request !\n";    

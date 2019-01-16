<?php

	// Make Uploads directory if not available
	if (!file_exists('uploads/')) {
	    mkdir('uploads/', 0777, true);
	}

	$target_dir = "uploads/";
	$target_file = $target_dir . basename($_FILES["picture"]["name"]);
	$uploadOk = 200;
	$imageFileType = pathinfo($target_file,PATHINFO_EXTENSION);
	$salt = generateRandomString();
	$target_file = $target_dir . $salt . "." . $imageFileType;
	
	// Check for fake or actual image
	if(isset($_POST["submit"])) {
	    $check = getimagesize($_FILES["picture"]["tmp_name"]);
	    if($check !== false) {
	        $uploadOk = 200;
	    } else {
	        // File is not an image
	        $uploadOk = 300;
	        $newURL = 'http://'.$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/" . "../" . "?status=" . $uploadOk;
			header('Location: '.$newURL);
	        die("300");
	    }
	}
	else {
		// Submit variable not set
		$uploadOk = 100;
		$newURL = 'http://'.$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/" . "../" . "?status=" . $uploadOk;
		header('Location: '.$newURL);
		die("100");
	}

	 // Check file size
	if ($_FILES["picture"]["size"] > 5242880) {
	    // File larger than 5MB (5,242,880 Bytes)
	    $uploadOk = 400;
	    $newURL = 'http://'.$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/" . "../" . "?status=" . $uploadOk;
		header('Location: '.$newURL);
	    die("400");
	}

	// Allow only JPG, JPEG, PNG & GIF
	if ($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg" && $imageFileType != "gif" ) {
		$uploadOk = 500;
		$newURL = 'http://'.$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/" . "../" . "?status=" . $uploadOk;
		header('Location: '.$newURL);
	    die("500");
	}	

	if ($uploadOk == 200) {
	    if (!(move_uploaded_file($_FILES["picture"]["tmp_name"], $target_file))) {
			// Unknown Error
			$newURL = 'http://'.$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/" . "../" . "?status=" . $uploadOk;
			header('Location: '.$newURL);
			die("600");
	    } 
    }

	function generateRandomString($length = 10) 
	{
		$characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
	    $charactersLength = strlen($characters);
	    $randomString = '';
	    for ($i = 0; $i < $length; $i++)
	    {
	        $randomString .= $characters[rand(0, $charactersLength - 1)];
	    }
	    return $randomString;
	}

	$parent = ucfirst(htmlspecialchars(utf8_encode($_POST["name"])));
	$child = ucfirst(htmlspecialchars(utf8_encode($_POST["cname"])));
	$email = ucfirst(htmlspecialchars(utf8_encode($_POST["email"])));
	$phone = ucfirst(htmlspecialchars(utf8_encode($_POST["phone"])));
	$country = ucfirst(htmlspecialchars(utf8_encode($_POST["country"])));
	$answerone = ucfirst(htmlspecialchars(utf8_encode($_POST["answer-one"])));
	$answertwo = ucfirst(htmlspecialchars(utf8_encode($_POST["answer-two"])));
	$answerthree = ucfirst(htmlspecialchars(utf8_encode($_POST["answer-three"])));


	// Check if CSV exists
	if (!file_exists("entries.csv")) {
		$file = fopen("entries.csv","w");

		$values = array("Parent's Name", "Child's Name", "Email Address", "Phone Number", "Country", "Question 1", "Question 2", "Question 3", "Uploaded Picture");
		fputcsv($file, $values);
		fclose($file);
	}

	// Append current entry to file
	$filePath = 'www.'.$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/".$target_file;
	$file = fopen("entries.csv","a");
	$values = array($parent, $child, $email, $phone, $country, $answerone, $answertwo, $answerthree, $filePath);
	fputcsv($file, $values);
	fclose($file);
	
	$newURL = 'http://'.$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF'])."/" . "../" . "?status=" . $uploadOk;
	header('Location: '.$newURL);
?>
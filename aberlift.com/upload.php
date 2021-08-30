<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<?php
	$target_dir = "uploads/";

	$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
	$imageFileType = pathinfo($target_file,PATHINFO_EXTENSION);
	$target_file = $target_dir . $_SESSION["UserID"] . "." . $imageFileType;

	$uploadOk = 1;
	$imageFileType = pathinfo($target_file,PATHINFO_EXTENSION);
	// Check if file has been selected
	if (!(basename($_FILES["fileToUpload"]["name"]) == ""))
	{
		//echo "Some file selected.";
		// Check if image file is a actual image or fake image
		if(isset($_POST["submit"])) {
		    $check = getimagesize($_FILES["fileToUpload"]["tmp_name"]);
		    if(!($check !== false)) {		// File is not an image.
		        $ErrorMessage = " Looks like the file you have selected is not an image.";
		        $uploadOk = 0;
		    }
		}

		// Allow certain file formats
		if($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg"
		&& $imageFileType != "gif" ) {
		    $ErrorMessage = " Only JPG, JPEG, PNG and GIF images are allowed.";
		    $uploadOk = 0;
		}
		// Check if $uploadOk is set to 0 by an error
		if ($uploadOk == 0) {
			// didn't try to upload the image as there were errors
		// if everything is ok, try to upload file
		} else {
		    if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
		    	$uploadOk = 1;

		    	/* Since it has been uploaded, resize it and replace it. */
		    	/* If JPG or JPEG */

		    	if ($imageFileType == "jpg" || $imageFileType == "jpeg")
		    	{
					$width = 250;
					$height = 250;

					// Content type
					header('Content-Type: image/jpeg');

					// Get new dimensions
					list($width_orig, $height_orig) = getimagesize($target_file);
					if (($width_orig > 250) && ($height_orig > 250))
					{					
						$ratio_orig = $width_orig/$height_orig;

						if ($width/$height > $ratio_orig) {
							$height = $width/$ratio_orig;
						} else {
							$width = $height*$ratio_orig;
						}

						// Resample
						$image_p = imagecreatetruecolor($width, $height);
						$image = imagecreatefromjpeg($target_file);
						imagecopyresampled($image_p, $image, 0, 0, 0, 0, $width, $height, $width_orig, $height_orig);

						// Output
						imagejpeg($image_p, $target_file);
					}
				}

		    	/* If GIF */

		    	else if ($imageFileType == "gif")
		    	{
					$width = 250;
					$height = 250;

					// Content type
					header('Content-Type: image/gif');

					// Get new dimensions
					list($width_orig, $height_orig) = getimagesize($target_file);
					if (($width_orig > 250) && ($height_orig > 250))
					{
						$ratio_orig = $width_orig/$height_orig;

						if ($width/$height > $ratio_orig) {
							$height = $width/$ratio_orig;
						} else {
							$width = $height*$ratio_orig;
						}

						// Resample
						$image_p = imagecreatetruecolor($width, $height);
						$image = imageCreateFromGif($target_file);

						 $background = imagecolorallocate($image_p, 0, 0, 0);
        				// removing the black from the placeholder
				        imagecolortransparent($image_p, $background);

						imagecopyresampled($image_p, $image, 0, 0, 0, 0, $width, $height, $width_orig, $height_orig);

						// Output
						imagegif($image_p, $target_file);
					}
				}

		    	/* If PNG */

		    	else if ($imageFileType == "png")
		    	{
					$width = 250;
					$height = 250;

					// Content type
					header('Content-Type: image/png');

					// Get new dimensions
					list($width_orig, $height_orig) = getimagesize($target_file);
					if (($width_orig > 250) && ($height_orig > 250))
					{					
						$ratio_orig = $width_orig/$height_orig;

						if ($width/$height > $ratio_orig) {
							$height = $width/$ratio_orig;
						} else {
							$width = $height*$ratio_orig;
						}

						// Resample
						$image_p = imagecreatetruecolor($width, $height);
						$image = imageCreateFromPng($target_file);

				        $background = imagecolorallocate($image_p, 0, 0, 0);
				        // removing the black from the placeholder
				        imagecolortransparent($image_p, $background);

				        // turning off alpha blending (to ensure alpha channel information 
				        // is preserved, rather than removed (blending with the rest of the 
				        // image in the form of black))
				        imagealphablending($image_p, false);

				        // turning on alpha channel information saving (to ensure the full range 
				        // of transparency is preserved)
				        imagesavealpha($image_p, true);

						imagecopyresampled($image_p, $image, 0, 0, 0, 0, $width, $height, $width_orig, $height_orig);

						// Output
						imagepng($image_p, $target_file);
					}
				}				
		    } else {
		        $uploadOk = 0;
		        $ErrorMessage = " An unknown error occurred while trying to upload the image.";
		    }
		}
	}


	/* end file checking */

	$formOk = 1;

	$firstName = ucfirst(htmlspecialchars(utf8_encode($_POST["fName"])));
	$lastName = ucfirst(htmlspecialchars(utf8_encode($_POST["lName"])));
	$phone = '44' . $string = str_replace(' ', '', htmlspecialchars(utf8_encode($_POST["phone"])));
	$userID = $_SESSION["UserID"];


	if (!(basename($_FILES["fileToUpload"]["name"]) == "") && ($uploadOk == 1))
	{
		$fileName = $_SESSION["UserID"] . "." . $imageFileType;
	}
	else
	{
		$fileName = "";
	}

	if ($firstName == "" || $firstName == null)
	{
		$ErrorMessage .= " First name cannot be empty.";
		$formOk = 0;
	}
	if ($lastName == "" || $lastName == null)
	{
		$ErrorMessage .= " Last name cannot be empty.";
		$formOk = 0;
	}
	if ($phone == "44" || $phone == null)
	{
		$ErrorMessage .= " Phone number cannot be empty!";
		$formOk = 0;
	}

	if ($uploadOk == 1 && $formOk == 1)
	{

		include 'DB_Connect.php';

		if (mysqli_connect_errno())
	  	{
			$ErrorMessage = " There was an error while contacting the server. Please try again.";
			header('Location: profile.php?Status=Error&Message=' . urlencode($ErrorMessage));
	  	}

		if ($fileName == "")
		{
			$SQL = "UPDATE AberLift_USERS SET firstName='$firstName', lastName='$lastName', phoneNumber='$phone' WHERE userID='$userID'";
		}
		else
		{
			$SQL = "UPDATE AberLift_USERS SET firstName='$firstName', lastName='$lastName', phoneNumber='$phone', profilePicture='$fileName' WHERE userID='$userID'";
		}
		if (!mysqli_query($con,$SQL))
		{
			$ErrorMessage = " There was an error while contacting the server. Please try again.";
			header('Location: profile.php?Status=Error&Message=' . urlencode($ErrorMessage));
		}
		else
		{
			$_SESSION["FirstName"] = $firstName;
			$_SESSION["LastName"] = $lastName;
			if (!($fileName == ""))
			{
				$_SESSION["Picture"] = $fileName . "?refresh=" . generateRandomString(2);
			}
			$_SESSION["PhoneNumber"] = $phone;

			header('Location: profile.php?Status=Success');
		}	
	}
	else
	{
		header('Location: profile.php?Status=Error&Message=' . urlencode($ErrorMessage));
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
?>
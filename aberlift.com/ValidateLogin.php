<?php
	$UserName = htmlspecialchars(utf8_encode($_POST["Username"]));
	$Password = htmlspecialchars(utf8_encode($_POST["Password"]));

	include 'DB_Connect.php';

	$Query = "SELECT * FROM AberLift_USERS WHERE emailAddress = '$UserName'";
	
	$Result = mysqli_query($con, $Query);

	if (@mysqli_num_rows($Result) == 0)
	{
		$Query = "SELECT * FROM AberLift_USERS_UNVERIFIED WHERE emailAddress = '$UserName'";
		
		$Result = mysqli_query($con, $Query);

		if (@mysqli_num_rows($Result) == 1)
		{
			echo "<span onclick=\"resendMail('" . $UserName . "')\" style='cursor:pointer' class='accent'> Resend mail?</span>";
		}
		else if (@mysqli_num_rows($Result) == 0)
		{
			echo "1";
		}
	}

	else
	{
		$row = mysqli_fetch_array($Result);
		$DB_Password = $row['password'];

		if($Password == $DB_Password)
		{
			// Start Session
			session_start();
			$_SESSION["UserID"] = $row['userID'];
			$_SESSION["FirstName"] = $row['firstName'];
			$_SESSION["LastName"] = $row['lastName'];
			$_SESSION["Picture"] = $row['profilePicture'];
			$_SESSION["PhoneNumber"] = $row['phoneNumber'];
			$_SESSION["EmailAddress"] = $row['emailAddress'];
			$_SESSION["LoggedIn"] = "Yes";

			echo "2";
		}
		else
		{
			echo "1";
		}
	}

?>
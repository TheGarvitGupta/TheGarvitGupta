<?php

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

	$timestamp = time();
	$FirstName = ucfirst(htmlspecialchars(utf8_encode($_POST["FName"])));
	$LastName = ucfirst(htmlspecialchars(utf8_encode($_POST["LName"])));
	$Phone = '44' . $string = str_replace(' ', '', htmlspecialchars(utf8_encode($_POST["Phone"])));
	$Email = htmlspecialchars(utf8_encode($_POST["EMail"]));
	$Password = htmlspecialchars(utf8_encode($_POST["Password"]));
	$VerificationCode = generateRandomString();

	if ($FirstName == "" || $LastName == "" || $Phone == "" || $Email == "" || $Password == "")
	{
		echo "3"; //Error in input
		exit();
	}
	/* ALLOW ALL FOR BETA
	if (!(substr($Email,-10) == "aber.ac.uk") && !(substr($Email,-14) == "garvitgupta.in") && !(substr($Email,-12) == "aberlift.com"))
	{
		exit();
	}
	*/
	include 'DB_Connect.php';

  	$SQL = "DELETE FROM AberLift_USERS_UNVERIFIED WHERE emailAddress='$Email'";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}

	$SQL = "SELECT IF(COUNT(*) > 0, 'true', 'false') AS 'Answer' FROM AberLift_USERS WHERE emailAddress = '$Email'";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}
	$Result = mysqli_query($con,$SQL);
	$row = mysqli_fetch_array($Result);
	$Answer = $row['Answer'];

	if($Answer == "true")
	{
		echo "2"; //Already registered and verified
		exit();
	}

  	$SQL = "INSERT INTO AberLift_USERS_UNVERIFIED VALUES('$timestamp', '$FirstName', '$LastName', '$Phone', '$Email', '$Password', '$VerificationCode')";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}

	$Text = "<div style=\"color:rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;font-size:14px;\"><div style=\"font-size:24px;\">Hi $FirstName" . ",</div><div style=\"margin-top:10px;\">Welcome to AberLift! You are just a step away from using the most popular platform for sharing rides. Just click on the following link to verify your account:</div><a href=\"http://beta.garvitgupta.com/Verify.php?K=$VerificationCode\" style=\"text-decoration:none;\"><div style=\"background-color: white; height:32px; border-radius: 5px; line-height:32px; color:rgba(0,0,0,0.70); font-family: Roboto, Helvetca, Arial; font-size: initial; padding-left:11px; padding-right:11px; text-align: center; box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24); width:175px; margin-top:50px; margin-bottom:50px; margin-left:auto; margin-right:auto; \">VERIFY ACCOUNT</div></a><div style=\"color:#2196F3;\">The AberLift Team</div></div><div style=\"font-size:12px;color:rgba(0,0,0,0.30);margin-top:25px;line-height:18px;-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;\">This is an automatically generated mail. Please do not reply. For support, please write to us at <a href=\"mailto:support@aberlift.com\">support@aberlift.com</a>. This email was sent to $Email. Please add no-reply@aberlift.com to your contacts so that our emails can reach to you without any troubles in the future.</div>";

	$Header .= "From: AberLift <no-reply@aberlift.com>\r\n";
	$Header .= "MIME-Version: 1.0\r\n";
	$Header .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

	mail($Email,"AberLift - Verify Account",$Text,$Header);

	echo "1";
?>
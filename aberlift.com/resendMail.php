<?php
	$Email = htmlspecialchars(utf8_encode($_POST["email"]));

	include 'DB_Connect.php';

	$SQL = "SELECT * FROM AberLift_USERS_UNVERIFIED WHERE emailAddress = '$Email'";
	$Result = mysqli_query($con, $SQL);

	if (@mysqli_num_rows($Result) == 1)
	{
		$row = mysqli_fetch_array($Result);

		$FirstName = $row['firstName'];
		$VerificationCode = $row['verificationCode'];

		$Text = "<div style=\"color:rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;font-size:14px;\"><div style=\"font-size:24px;\">Hi $FirstName" . ",</div><div style=\"margin-top:10px;\">Welcome to AberLift! You are just a step away from using the most popular platform for sharing rides. Just click on the following link to verify your account:</div><a href=\"http://www.aberlift.com/Verify.php?K=$VerificationCode\" style=\"text-decoration:none;\"><div style=\"background-color: white; height:32px; border-radius: 5px; line-height:32px; color:rgba(0,0,0,0.70); font-family: Roboto, Helvetca, Arial; font-size: initial; padding-left:11px; padding-right:11px; text-align: center; box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24); width:175px; margin-top:50px; margin-bottom:50px; margin-left:auto; margin-right:auto; \">VERIFY ACCOUNT</div></a><div style=\"color:#2196F3;\">The AberLift Team</div></div><div style=\"font-size:12px;color:rgba(0,0,0,0.30);margin-top:25px;line-height:18px;-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;\">This is an automatically generated mail. Please do not reply. For support, please write to us at <a href=\"mailto:support@aberlift.com\">support@aberlift.com</a>. This email was sent to $Email. Please add no-reply@aberlift.com to your contacts so that our emails can reach to you without any troubles in the future.</div>";
		
		$Header .= "From: AberLift <no-reply@aberlift.com>\r\n";
		$Header .= "MIME-Version: 1.0\r\n";
		$Header .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

		mail($Email,"AberLift - Verify Account",$Text,$Header);

		echo "1";
	}
	else
	{
		echo "Some error occured.";
	}
?>
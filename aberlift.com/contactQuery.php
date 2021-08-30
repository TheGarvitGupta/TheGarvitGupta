<?php
	$name = htmlspecialchars(utf8_encode($_POST["name"]));
	$subject = htmlspecialchars(utf8_encode($_POST["subject"]));
	$email = htmlspecialchars(utf8_encode($_POST["email"]));
	$message = htmlspecialchars(utf8_encode($_POST["message"]));


	$Text = "<div>$message</div>\n\n<div>$name</div>\n<div>$email</div>";

	$Header = "From: AberLift <no-reply@aberlift.com>\r\n";
	$Header .= "MIME-Version: 1.0\r\n";
	$Header .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

	//mail("support@aberlift.com",$subject,$Text,$Header);
	if (!($message == "" && $subject == "" && $email == "" && $name == ""))
	{
		mail("me@garvitgupta.in",$subject,$Text,$Header);
	}

	echo "1";
?>
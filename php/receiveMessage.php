<?php

	$name = htmlspecialchars(utf8_encode($_POST["Name"]));
	$email = htmlspecialchars(utf8_encode($_POST["Email"]));
	$number = htmlspecialchars(utf8_encode($_POST["Number"]));
	$message = htmlspecialchars(utf8_encode($_POST["Message"]));
	$intentApp = htmlspecialchars(utf8_encode($_POST["App"]));
	$intentWeb = htmlspecialchars(utf8_encode($_POST["Web"]));
	$intentJob = htmlspecialchars(utf8_encode($_POST["Job"]));
	$intentFeedback = htmlspecialchars(utf8_encode($_POST["Feedback"]));

	$Text = "Hello Garvit,<br><br>$message<br><br>Intent of this message:<br><br>";

	if ($intentApp == "1")
	{
		$Text .= "- Mobile App<br>";
	}
	
	if ($intentWeb == "1")
	{
		$Text .= "- Website<br>";
	}
	
	if ($intentJob == "1")
	{
		$Text .= "- Work<br>";
	}
	
	if ($intentFeedback == "1")
	{
		$Text .= "- Feedback<br>";
	}

	$Text .= "<br>$name<br>$number<br>$email";

	$Header = "From:$name<no-reply@garvitgupta.com>\r\n";
	$Header .= "MIME-Version: 1.0\r\n";
	$Header .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

	mail("garvit@ymail.com","You have a new message, Garvit!",$Text,$Header);

	echo "1";
?>
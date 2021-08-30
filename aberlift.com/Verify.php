<meta charset="UTF-8">
<html>
<head>
	<link href="Resources/css/home.css" rel="stylesheet" type="text/css">
	<style> /* Message Window Styles */
	#messageWindow-full
	{
		position:fixed;
		width:100%;
		height:100%;
		left:0;
		right:0;
		top:0;
		bottom:0;

		opacity:0;

		transition:0.25s ease-out;
		transition-delay:0.25s;

		background-color:rgba(0,0,0,0.1);
		z-index:1;
	}
	#message-Window
	{
		height:450px;
		width:800px;
		margin-top:150px;
		margin-left:auto;
		margin-right:auto;
		background-color:white;
		transition:0.25s ease-out;
		transition-delay:0.25s;
	}
	</style>
	<script> /*Message Window Script */
	function showPopUp()
	{
		var elementExists = document.getElementById("messageWindow-full");
		if (elementExists)
		{
			setTimeout(function(){
				document.getElementById("messageWindow-full").style.opacity = "1";
				document.getElementById("message-window").style.boxShadow = "rgba(0, 0, 0, 0.298039) 0px 19px 38px, rgba(0, 0, 0, 0.219608) 0px 15px 12px";
			}, 100);
		}
	}
	</script>
</head>
<body onload="showPopUp()"> 
	<?php include 'header.php'; ?>
	<?php
		$Key = htmlspecialchars(utf8_encode($_GET["K"]));

		include 'DB_Connect.php';

		$Query = "SELECT * FROM AberLift_USERS_UNVERIFIED WHERE verificationCode='$Key'";
		
		$Result = mysqli_query($con, $Query);
		
		if (@mysqli_num_rows($Result) == 0)
		{
			// Invalid key
			echo '<div id="messageWindow-full">';
			echo '	<div id="message-window">';
			echo '		<img src="Resources/svg/error.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;">';
			echo '		<div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px">';
			echo '			<div style="text-align:center;font-size:18px;line-height:32px"class="accent">Invalid key</div>';
			echo '			<br>It seems that this verification key is invalid. This might be because the account has already been activated or the verification email has expired. Please sign up again if it\'s been more than thirty minutes since you signed up.';
			echo '		</div>';
			echo '	</div>';
			echo '</div>';
		}

		else if(@mysqli_num_rows($Result) == 1)
		{

			$row = mysqli_fetch_array($Result);

			$timeStamp = time();
			$userID = generateRandomString();
			$fName = $row['firstName'];
			$lName = $row['lastName'];
			$phone = $row['phoneNumber'];
			$email = $row['emailAddress'];
			$password = $row['password'];

			$SQL = "DELETE FROM AberLift_USERS_UNVERIFIED WHERE verificationCode='$Key'";
			if (!mysqli_query($con,$SQL))
			{
				echo("Error description: " . mysqli_error($con));
			}

			$SQL = "INSERT INTO AberLift_USERS VALUES('$timeStamp', '$userID', '$fName', '$lName', '$phone', '$email', 'null', '$password')";
			if (!mysqli_query($con,$SQL))
			{
				echo("Error description: " . mysqli_error($con));
			}
			else
			{
				// verified
				echo '<div id="messageWindow-full">';
				echo '	<div id="message-window">';
				echo '		<img src="Resources/svg/done_all.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;">';
				echo '		<div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px">';
				echo '			<div style="text-align:center;font-size:18px;line-height:32px"class="accent">Welcome aboard!</div>';
				echo '			<br>You account has successfully been activated. You can now start using AberLift. Please sign in with your registered email and password and update your profile.';
				echo '		</div>';
				echo '	</div>';
				echo '</div>';
			}
		}

		else
		{
			echo '<div id="messageWindow-full">';
			echo '	<div id="message-window">';
			echo '		<img src="Resources/svg/error.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;">';
			echo '		<div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px">';
			echo '			<div style="text-align:center;font-size:18px;line-height:32px"class="accent">Error</div>';
			echo '			<br>An unknown error occured while trying to activate your account. Please sign up again with the same email account. If nothing works, you can contact us directly by using the link on the footer.';
			echo '		</div>';
			echo '	</div>';
			echo '</div>';
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
	<div style="margin-top:600px;"></div>
	<?php include 'footer.php';?>
	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300,100' rel='stylesheet' type='text/css'>
</body>
</html>
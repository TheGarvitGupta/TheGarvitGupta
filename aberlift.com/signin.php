<?php
	session_start();
	if (isset($_SESSION["LoggedIn"]))
	{
		header('Location: index.php');
	}
?>
<meta charset="UTF-8">
<html>
<head>
	<link href="Resources/css/home.css" rel="stylesheet" type="text/css">
	<link href="Resources/css/spinner.css" rel="stylesheet" type="text/css">
	<script src='Resources/JS/searchOfferRides.js'></script> 
<style>
.sign-content
{
	margin-top:65px;
	width:100%;
	padding-top:50px;
	padding-bottom:50px;
}
.sub-sign-content
{
	width:400px;
}
.center
{
	margin-left:auto;
	margin-right:auto;
	display: block;
}
.button
{

}
.sign-in-objects
{
	margin-top:25px;
}
.accent
{
	color:#2196F3;
}
#button-container
{
	height:32px;
}
</style>
<script>
	function SignIn()
	{
		document.getElementById("username-error").style.opacity = "0";
		document.getElementById("password-error").style.opacity = "0";
		document.getElementById("login-error").style.opacity = "0";
		document.getElementById("login-error").style.lineHeight = "2.2em";

		var username = document.getElementById("username").value;
		var password = document.getElementById("password").value;

		var validInputs = 1;

		if (username == "")
		{
			document.getElementById("username-error").innerHTML = "Please enter your AberLift Email Address";
			document.getElementById("username-error").style.opacity = "1";
			validInputs = 0;
		}
		else if (!(/^\w+([\.-]?\w+)*@\w+([\.-]?\w+)*(\.\w{2,3})+$/.test(username)))
		{
			document.getElementById("username-error").innerHTML = "This looks like an invalid Email Address";
			document.getElementById("username-error").style.opacity = "1";
			validInputs = 0;
		}

		if (password == "")
		{
			document.getElementById("password-error").innerHTML = "Please enter your AberLift Password";
			document.getElementById("password-error").style.opacity = "1";
			validInputs = 0;
		}


		if (validInputs == 1)
		{
			document.getElementById("button-container").innerHTML = "<svg class='center spinner' style='margin-top:15px' width='32px' height='32px' viewBox='0 0 66 66' xmlns='http://www.w3.org/2000/svg'><circle class='path' fill='none' stroke-width='6' stroke-linecap='round' cx='33' cy='33' r='30'></circle></svg>";

			xmlhttp = new XMLHttpRequest();
			xmlhttp.onreadystatechange = function () {
				if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
					var Reply = xmlhttp.responseText;
					if(Reply == '1') // Invalid Email Password
					{	
						document.getElementById("button-container").innerHTML = "<div onclick='SignIn()' class='center header-button' style='width:70px;margin-top:15px'>Sign In</div>";
						document.getElementById("login-error").innerHTML = "The email or password you entered is incorrect.";
						document.getElementById("login-error").style.opacity = "1";
					}
					else if(Reply == '2') // Correct and Session Started - User Logged In
					{
						document.getElementById("button-container").innerHTML = "<div onclick='SignIn()' class='center header-button' style='width:70px;margin-top:15px'>Sign In</div>";
						window.location="index.php";
					}
					else // Not verified
					{	
						document.getElementById("button-container").innerHTML = "<div onclick='SignIn()' class='center header-button' style='width:70px;margin-top:15px'>Sign In</div>";
						document.getElementById("login-error").innerHTML = "You need to verify your Email Address before you can Sign In. Please click on the link in your mail to verify." + Reply;
						document.getElementById("login-error").style.lineHeight = "1.5em";
						document.getElementById("login-error").style.opacity = "1";
					}

				}
			}
			xmlhttp.open("POST","ValidateLogin.php",true);
			xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
			xmlhttp.send("Username=" + username + "&Password=" + password);
		}
	}
	function usernameVerify()
	{
		document.getElementById("username-error").style.opacity = "0";
		var username = document.getElementById("username").value;

		if (username == "")
		{
			document.getElementById("username-error").innerHTML = "Please enter your AberLift Email Address";
			document.getElementById("username-error").style.opacity = "1";
			validInputs = 0;
		}
		else if (!(/^\w+([\.-]?\w+)*@\w+([\.-]?\w+)*(\.\w{2,3})+$/.test(username)))
		{
			document.getElementById("username-error").innerHTML = "This looks like an invalid Email Address";
			document.getElementById("username-error").style.opacity = "1";
			validInputs = 0;
		}
	}
	function passwordVerify()
	{
		document.getElementById("password-error").style.opacity = "0";
		var password = document.getElementById("password").value;

		if (password == "")
		{
			document.getElementById("password-error").innerHTML = "Please enter your AberLift Password";
			document.getElementById("password-error").style.opacity = "1";
			validInputs = 0;
		}
	}
	function TryInput(e)
	{
	    if (typeof e == 'undefined' && window.event) 
	    {
	        e = window.event;
	    }
	    if (e.keyCode == 13) 
	    {
	        SignIn();
	    }
	}
</script>
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
	display:none;

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
<script>
	function resendMail(emailAddress)
	{
		document.getElementById("button-container").innerHTML = "<svg class='center spinner' style='margin-top:15px' width='32px' height='32px' viewBox='0 0 66 66' xmlns='http://www.w3.org/2000/svg'><circle class='path' fill='none' stroke-width='6' stroke-linecap='round' cx='33' cy='33' r='30'></circle></svg>";
	
		xmlhttp = new XMLHttpRequest();
		xmlhttp.onreadystatechange = function () {
			if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
				var Reply = xmlhttp.responseText;
				if (Reply == '1')
				{	
					document.getElementById("message-window").innerHTML = '<img src="Resources/svg/verify.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;"><div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px"><div style="text-align:center;font-size:18px;line-height:32px"class="accent">Please verify your account</div><br>We have sent a verification link on your email address ' + emailAddress + '. Please click on the link to verify your account and start using AberLift.</div>   <div onclick="closeMessage()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;">close</div>';
					document.getElementById("messageWindow-full").style.display = "block";
					
					setTimeout(function(){
						document.getElementById("messageWindow-full").style.opacity = "1";
						document.getElementById("message-window").style.boxShadow = "rgba(0, 0, 0, 0.298039) 0px 19px 38px, rgba(0, 0, 0, 0.219608) 0px 15px 12px";
						document.getElementById("button-container").innerHTML = "<div onclick='SignIn()' class='center header-button' style='width:70px;margin-top:15px'>Sign In</div>";
					}, 250);
				}
				else
				{
					alert(Reply);
				}
			}
		}
		xmlhttp.open("POST","resendMail.php",true);
		xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
		xmlhttp.send("email=" + emailAddress);
	}

	function closeMessage()
	{
		document.getElementById("message-window").style.boxShadow = "0px 0px 0px rgb(0,0,0)";
		document.getElementById("messageWindow-full").style.opacity = "0";
		setTimeout(function(){ 	document.getElementById("message-window").innerHTML = ""; document.getElementById("messageWindow-full").style.display = "none"; }, 500);
	}
</script>
</head>
<body>
	<?php
		include 'header.php';
	?>

	<div id="messageWindow-full">
		<div id="message-window"></div>
	</div>

	<div class="sign-content">
		<div class="center sub-sign-content">
			<input onkeypress="TryInput(event)" id="username" onblur="usernameVerify()" type="text" class="object center sign-in-objects" style="width:400px" placeholder="Email Address"></input>
			<div id="username-error" class="error"></div>
			<input onkeypress="TryInput(event)" id="password" onblur="passwordVerify()" type="password" class="object center sign-in-objects" style="width:400px" placeholder="Password"></input>
			<div id="password-error" class="error"></div>

			<div id="login-error" class="error" style="position:relative;margin-top:15px;text-align:center;min-height:2.2em"></div>
			<div id="button-container">
				<div onclick='SignIn()' class='center header-button' style='width:70px;margin-top:15px'>Sign In</div>
			</div>
			<div class="hint" style="margin-top:50px;line-height:0.85714285714286em;">Don't have an account? <a href="signup.php"><span tabindex="0" class="accent">Sign Up</span></a></div>
			<!--<div class="hint" style="margin-top:10px;line-height:0.85714285714286em;"><a href="signup.php"><span tabindex="0" class="accent">Forgot Password?</span></a></div>-->
		</div>
	</div>
	<?php include 'footer.php' ?>
	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300,100' rel='stylesheet' type='text/css'>
</body>
</html>
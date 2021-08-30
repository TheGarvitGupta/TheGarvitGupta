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
	<script src='Resources/JS/jquery.min.js'></script> 
	<script src="Resources/JS/jquery.maskedinput.min.js" type="text/javascript"></script>
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
		#button-container
		{
			height:32px;
		}
		.bg-44
		{
			background:url('Resources/images/+44.png');
			background-position:5px 16px;
			background-repeat: no-repeat;
		}
	</style>
	<script>

		$(document).ready(function() {
			$('#firstName').keypress(function (e) {
			    var regex = new RegExp("^[a-zA-Z_ ]*$");
			    var str = String.fromCharCode(!e.charCode ? e.which : e.charCode);
			    if (regex.test(str)) {
			        return true;
			    }

			    e.preventDefault();
			    return false;
			});
		});

		$(document).ready(function() {
			$('#lastName').keypress(function (e) {
			    var regex = new RegExp("^[a-zA-Z_ ]*$");
			    var str = String.fromCharCode(!e.charCode ? e.which : e.charCode);
			    if (regex.test(str)) {
			        return true;
			    }

			    e.preventDefault();
			    return false;
			});
		});

		$(document).ready(function() {
			$('#firstName', '#lastName', '#password', '#repeatPassword').keypress(function() {
			    if($(this).val().length >= 25) {
			        $(this).val($(this).val().slice(0, 25));
			        return false;
			    }
			});
		});

		$(document).ready(function() {
			$('#emailAddress').keypress(function() {
			    if($(this).val().length >= 35) {
			        $(this).val($(this).val().slice(0, 35));
			        return false;
			    }
			});
		});		

		$(document).ready(function() {
			$('#phone').mask("999 999 9999",{placeholder:" "});
		});

		function SignUp()
		{
			document.getElementById("firstName-error").style.opacity = "0";
			document.getElementById("lastName-error").style.opacity = "0";
			document.getElementById("phone-error").style.opacity = "0";
			document.getElementById("emailAddress-error").style.opacity = "0";
			document.getElementById("password-error").style.opacity = "0";
			document.getElementById("repeatPassword-error").style.opacity = "0";

			var fname = document.getElementById("firstName").value;
			var lname = document.getElementById("lastName").value;
			var phone = document.getElementById("phone").value;
			var email = document.getElementById("emailAddress").value;
			var password = document.getElementById("password").value;
			var cpassword = document.getElementById("repeatPassword").value;

			var validInputs = 1;

			if (password == "")
			{
				document.getElementById("password-error").innerHTML = "Please enter a password";
				document.getElementById("password-error").style.opacity = "1";
				document.getElementById("password-error").style.color = "rgba(248,0,23,1)";
				validInputs = 0;
			}
			if (cpassword == "")
			{
				document.getElementById("repeatPassword-error").innerHTML = "Please enter the password again";
				document.getElementById("repeatPassword-error").style.opacity = "1";
				document.getElementById("repeatPassword-error").style.color = "rgba(248,0,23,1)";
				validInputs = 0;
			}

			if (!isStrong(password) && (password!=""))
			{
				if (password.length < 6)
				{
					document.getElementById("password-error").innerHTML = "Password must have atleast 6 characters";
				}
				else
				{
					document.getElementById("password-error").innerHTML = "Password must contain atleast one digit";
				}
				document.getElementById("password-error").style.opacity = "1";
				document.getElementById("password-error").style.color = "rgba(248,0,23,1)";
				validInputs = 0;
			}
			if ((password != cpassword) && (cpassword!=""))
			{
				validInputs = 0;
				document.getElementById("repeatPassword-error").innerHTML = "Please enter the same password here";
				document.getElementById("repeatPassword-error").style.opacity = "1";
			}

			if (fname == "")
			{
				document.getElementById("firstName-error").style.opacity = "1";
				document.getElementById("firstName-error").style.color = "rgba(248,0,23,1)";
				validInputs = 0;
			}
			if (lname == "")
			{
				document.getElementById("lastName-error").style.opacity = "1";
				document.getElementById("lastName-error").style.color = "rgba(248,0,23,1)";
				validInputs = 0;
			}
			if (phone == "")
			{
				document.getElementById("phone-error").style.opacity = "1";
				document.getElementById("phone-error").style.color = "rgba(248,0,23,1)";
				validInputs = 0;
			}
			if (email == "")
			{
				document.getElementById("emailAddress-error").innerHTML = "Please enter your email address";
				document.getElementById("emailAddress-error").style.opacity = "1";
				document.getElementById("emailAddress-error").style.color = "rgba(248,0,23,1)";
				validInputs = 0;
			}
			else if (!(/^\w+([\.-]?\w+)*@\w+([\.-]?\w+)*(\.\w{2,3})+$/.test(email)))
			{
				document.getElementById("emailAddress-error").innerHTML = "This looks like an invalid email address";
				document.getElementById("emailAddress-error").style.opacity = "1";
				document.getElementById("emailAddress-error").style.color = "rgba(248,0,23,1)";
				validInputs = 0;
			}
			/* ALLOW ALL FOR BETA 
			else if (!(email.slice(-11) == "@aber.ac.uk") && !(email.slice(-15) == "@garvitgupta.in") && !(email.slice(-13) == "@aberlift.com"))
			{
				document.getElementById("emailAddress-error").innerHTML = "Email address must end with @aber.ac.uk";
				document.getElementById("emailAddress-error").style.opacity = "1";
				document.getElementById("emailAddress-error").style.color = "rgba(248,0,23,1)";
				validInputs = 0;
			}
			*/
			if (validInputs == 1)
			{
				document.getElementById("button-container").innerHTML = "<svg class='center spinner' style='margin-top:50px' width='32px' height='32px' viewBox='0 0 66 66' xmlns='http://www.w3.org/2000/svg'><circle class='path' fill='none' stroke-width='6' stroke-linecap='round' cx='33' cy='33' r='30'></circle></svg>";
			
				xmlhttp = new XMLHttpRequest();
				xmlhttp.onreadystatechange = function () {
					if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
						var Reply = xmlhttp.responseText;
						if (Reply == '1')
						{
							document.getElementById("message-window").innerHTML = '<img src="Resources/svg/verify.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;"><div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px"><div style="text-align:center;font-size:18px;line-height:32px"class="accent">Please verify your account</div><br>We have sent a verification link on your email address ' + email + '. Please click on the link to verify your account and start using AberLift.</div>   <div onclick="closeMessage()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;">close</div>';
							document.getElementById("messageWindow-full").style.display = "block";
							
							setTimeout(function(){
								document.getElementById("messageWindow-full").style.opacity = "1";
								document.getElementById("message-window").style.boxShadow = "rgba(0, 0, 0, 0.298039) 0px 19px 38px, rgba(0, 0, 0, 0.219608) 0px 15px 12px";
								document.getElementById("button-container").innerHTML = "<div onclick='SignUp()' class='center header-button' style='width:70px;margin-top:50px'>Sign Up</div>";
							}, 250);
						}
						else if (Reply == '2') //verify
						{	
							document.getElementById("message-window").innerHTML = '<img src="Resources/svg/warning.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;"><div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px"><div style="text-align:center;font-size:18px;line-height:32px"class="accent">ID already in use</div><br>This email account has already been used to register on AberLift. Please user your AberLift password to log in.</div><div onclick="closeMessage()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;">close</div>';
							document.getElementById("messageWindow-full").style.display = "block";
							
							setTimeout(function(){
								document.getElementById("messageWindow-full").style.opacity = "1";
								document.getElementById("message-window").style.boxShadow = "rgba(0, 0, 0, 0.298039) 0px 19px 38px, rgba(0, 0, 0, 0.219608) 0px 15px 12px";
								document.getElementById("button-container").innerHTML = "<div onclick='SignUp()' class='center header-button' style='width:70px;margin-top:50px'>Sign Up</div>";
							}, 250);
						}
						else // wrong inputs
						{
							document.getElementById("message-window").innerHTML = '<img src="Resources/svg/error.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;"><div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px"><div style="text-align:center;font-size:18px;line-height:32px"class="accent">Ouch! An error occured</div><br>There seems to be an error in the data we\'ve received from you. Please refresh this page and try again.</div><div onclick="closeMessage()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;">close</div>';
							document.getElementById("messageWindow-full").style.display = "block";

							setTimeout(function(){
								document.getElementById("messageWindow-full").style.opacity = "1";
								document.getElementById("message-window").style.boxShadow = "rgba(0, 0, 0, 0.298039) 0px 19px 38px, rgba(0, 0, 0, 0.219608) 0px 15px 12px";
								document.getElementById("button-container").innerHTML = "<div onclick='SignUp()' class='center header-button' style='width:70px;margin-top:50px'>Sign Up</div>";
							}, 250);
						}
					}
				}
				xmlhttp.open("POST","signUpScript.php",true);
				xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
				xmlhttp.send("FName=" + fname  + "&LName=" + lname + "&Password=" + password + "&Phone=" + phone + "&EMail=" + email);
			}
		}

		function isStrong(str)
		{
			//var re = /^(?=.*\d)(?=.*[a-z])(?=.*[A-Z]).{6,}$/;
			var re = /^(?=.*\d).{6,}$/;
			return re.test(str);
		}

		function closeMessage()
		{
			document.getElementById("message-window").style.boxShadow = "0px 0px 0px rgb(0,0,0)";
			document.getElementById("messageWindow-full").style.opacity = "0";
			setTimeout(function(){ 	document.getElementById("message-window").innerHTML = ""; document.getElementById("messageWindow-full").style.display = "none"; }, 500);
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
</head>
<body>
	<?php
		include 'header.php';
	?>

	<div id="messageWindow-full">
		<div id="message-window">
		</div>
	</div>

	<div class="sign-content">
		<div class="center sub-sign-content">
			<input id="firstName" type="text" class="object center sign-in-objects" style="width:400px" placeholder="First Name" maxlength="25"></input>
			<div id="firstName-error" class="error">Please enter your first name</div>

			<input id="lastName" type="text" class="object center sign-in-objects" style="width:400px" placeholder="Last Name" maxlength="25"></input>
			<div id="lastName-error" class="error">Please enter your last name</div>

			<input id="phone" type="text" class="bg-44 object center sign-in-objects" style="width:400px;padding-left:45px" placeholder="Phone Number" maxlength="10"></input>
			<div id="phone-error" class="error">Please enter your phone number</div>

			<input id="emailAddress" type="text" class="object center sign-in-objects" style="width:400px" placeholder="Email Address" maxlength="35"></input>
			<div id="emailAddress-error" class="error">Please enter your email address</div>			

			<input id="password" type="password" class="object center sign-in-objects" style="width:400px" placeholder="Password" maxlength="25"></input>
			<div id="password-error" class="error">Please enter a password</div>

			<input id="repeatPassword" type="password" class="object center sign-in-objects" style="width:400px" placeholder="Confirm Password" maxlength="25"></input>
			<div id="repeatPassword-error" class="error">Please enter the password again</div>			
			
			<div id="button-container">
				<div onclick="SignUp()" class="center header-button" style="width:70px;margin-top:50px">Sign Up</div>
			</div>
			<div class="hint" style="margin-top:50px">Already have an account? <a href="signin.php"><span class="accent">Sign In</span></a></div>
		</div>
	</div>
	<?php include 'footer.php' ?>
	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300,100' rel='stylesheet' type='text/css'>
</body>
</html>
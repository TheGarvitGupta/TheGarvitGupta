var menuStatus = "Hidden";

function toggleMenu()
{
	if (menuStatus == "Hidden")
	{
		showMenu();
	}
	else
	{
		hideMenu();
	}
}
function showMenu()
{
	menuStatus = "Visible";
	document.getElementById("boxContainer").style.display = "block";
	document.getElementById("headerScrollBox").style.top = "65px";
}
function hideMenu()
{
	menuStatus = "Hidden";
	document.getElementById("boxContainer").style.display = "none";
	document.getElementById("headerScrollBox").style.top = "-75px";	
}
var action = "get";

function resetSearch(check)
{
	document.getElementById("searchContainer").style.opacity = "0";
	document.getElementById("get-hbdsdf-error").style.opacity = "0";
	document.getElementById("get-board-error").style.opacity = "0";
	document.getElementById("get-destination-error").style.opacity = "0";
	document.getElementById("get-time-error").style.opacity = "0";

	var delay=250;
    setTimeout(function(){
		document.getElementById("searchContainer").style.opacity = "1";

	    document.getElementById("hbdsdf").value = "Date";
	    document.getElementById("get-board").value = "";
	    document.getElementById("get-destination").value = "";
		document.getElementById("get-time").value = "";

		if (check == "get")
		{
			document.getElementById("form-submit").style.backgroundImage = "url('Resources/images/Material_Right.png')";
		}
		else
		{
			document.getElementById("form-submit").style.backgroundImage = "url('Resources/images/Material_Search.png')";
		}
	},delay); 
}
function showOffer()
{
	document.getElementById("sign-up-button").style.backgroundColor = "#2196F3";
	document.getElementById("sign-in-button").style.backgroundColor = "#ffffff";
	if (action != "offer")
	{
		resetSearch("get");
	}
	action = "offer";
}
function showGet()
{
	document.getElementById("sign-up-button").style.backgroundColor = "#ffffff";
	document.getElementById("sign-in-button").style.backgroundColor = "#2196F3";
	if (action != "get")
	{
		resetSearch("offer");
	}
	action = "get";
}
function searchRides()
{
	document.getElementById('get-board-error').style.opacity = "0"; 
	document.getElementById('get-destination-error').style.opacity = "0"; 
	document.getElementById('get-hbdsdf-error').style.opacity = "0"; 
	document.getElementById('get-time-error').style.opacity = "0"; 

	var boardingPoint = document.getElementById('get-board').value;
	var destination = document.getElementById('get-destination').value;
	var boardingDate = document.getElementById('hbdsdf').value;
	var boardingTime = document.getElementById('get-time').value;

	var flag = 0;

	if (boardingPoint == "")
	{
		document.getElementById('get-board-error').style.opacity = "1";
		flag++;
	}
	if (destination == "")
	{
		document.getElementById('get-destination-error').style.opacity = "1"; 
		flag++;
	}
	if (boardingDate.localeCompare("Date") == 1)
	{
		document.getElementById('get-hbdsdf-error').style.opacity = "1";
		flag++;
	}
	if (boardingDate == "Date")
	{
		document.getElementById('get-hbdsdf-error').style.opacity = "1";
		flag++;
	}
	if (boardingTime == "")
	{
		document.getElementById('get-time-error').style.opacity = "1";
		flag++;
	}

	if(flag == 0)
	{
		if (action == "offer")
		{
			window.location="offerARide.php?From=" + boardingPoint + "&To=" + destination + "&Date=" + boardingDate + "&Time=" + boardingTime + "&Capacity=1";
		}
		else if (action == "get")
		{
			window.location="getARide.php?From=" + boardingPoint + "&To=" + destination + "&Date=" + boardingDate + "&Time=" + boardingTime + "&Capacity=1";
		}
	}
}

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

/* Fix Locations */
function fixDestination()
{
	var boardingPoint = document.getElementById('get-board').value;
	var op = document.getElementById("get-destination").getElementsByTagName("option");
	for (var i = 0; i < op.length; i++)
	{
  		if(op[i].value == boardingPoint) 
  		{
  			op[i].disabled = true;
  		}
  		else
  		{
  			op[i].disabled = false;
  		}
	}

	javascript:document.getElementById("get-board").getElementsByTagName("option")[0].disabled = true;
	javascript:document.getElementById("get-destination").getElementsByTagName("option")[0].disabled = true;
}
function fixBoarding()
{
	var destination = document.getElementById('get-destination').value;
	var op = document.getElementById("get-board").getElementsByTagName("option");
	for (var i = 0; i < op.length; i++)
	{
  		if(op[i].value == destination) 
  		{
  			op[i].disabled = true;
  		}
  		else
  		{
  			op[i].disabled = false;
  		}
	}
	op[0].disabled = true;

	javascript:document.getElementById("get-board").getElementsByTagName("option")[0].disabled = true;
	javascript:document.getElementById("get-destination").getElementsByTagName("option")[0].disabled = true;
}

$(document).ready(function () {
    $(window).scroll(function () {
        $('.hideme').each(function (i) {

            var bottom_of_object = $(this).offset().top + $(this).outerHeight();
            var bottom_of_window = $(window).scrollTop() + $(window).height();

            if (bottom_of_window > bottom_of_object) {
                $(this).css({opacity: 1});
            }
        });
    });
});
flag_survey="<% ConfigGet("/uplink/extendersite/survey"); %>";
siteList = [<% ConfigGetArray("/wirelesssitesurvey/site#", 
				"wirelesstype",
				"bssid",
				"wirelessname",
				"encryptionmode",
				"wirelessstrength"); %>];

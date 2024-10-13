<html>
<head>
<title>RssSet</title>
</head>
	<script>
		  function btnApply()
    {
//      if (uiDoValidate()==false) return;
        with ( document.forms[0] ) {    
            document.forms[0].submit();
            return true;
        }
    }
	</script>
<body>
<h1>RssSet Upload Test</h1>
<form ENCTYPE="multipart/form-data" method="POST" name="uiPostUpdateForm" id="uiPostUpdateForm" action="rssset_cloning.xgi">
	<table cellpadding="1" cellspacing="1" border="0" width="525">  Select file: 
		<tr> <td class=bc_tb colspan=2><input type='file' name='tools_FW_UploadFile' size='30'></td> </tr>
		<tr> <td colspan=2>&nbsp;</td> </tr>
		<tr>
			<td width=50%></td>
			<td class=bl_tb><input type=button name="update" value="Upload" onclick="btnApply();"></td>
		</tr>
	</table>
</form>
</body>
</html>

<form  action="{ScriptPath}" method="POST" title="�����">
  <p>�����: <input type="text" name="QueryText" size="80" maxlength="254" alt="�����" tabindex="0" value="{ValueQueryText}" /><br />

<!-- BEGIN blockOnlyPathSearch -->
	<input type="checkbox" name="OnlyPathSearch" alt="������ ������ � ��������� ���������" tabindex="1"{CheckedOnlyPathSearch} />������ � ��������� ���������<br />
<!-- END blockOnlyPathSearch -->

<!-- BEGIN blockfilters -->
	��� ������� ������:<br />
<!-- BEGIN filters -->
	<input type="checkbox" name="{filters.NAME}" alt="{filters.ALT}" tabindex="{filters.TABINDEX}"{filters.Checked} />{filters.ALT}&nbsp;
<!-- END filters -->
	<br />
<!-- END blockfilters -->

<!-- BEGIN blockOnlyActive -->
	<input type="checkbox" name="OnlyActive" alt="�������� ������ �������� ������" tabindex="{Tab_OnlyActive_Id}"{CheckedOnlyActive} />�������� ������ �������� ������
<!-- END blockOnlyActive -->

	</p>
  <input type="reset" value="�������� �����" tabindex="{Tab_Reset_Id}" />&nbsp;
	<input type="submit" value='�����' tabindex="{Tab_Submit_Id}" />
</form>

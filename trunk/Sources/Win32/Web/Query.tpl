<form  action="{ScriptPath}" method="POST" title="Поиск">
  <p>Поиск: <input type="text" name="QueryText" size="80" maxlength="254" alt="Поиск" tabindex="0" value="{ValueQueryText}" /><br />

<!-- BEGIN blockOnlyPathSearch -->
	<input type="checkbox" name="OnlyPathSearch" alt="Искать только в названиях каталогов" tabindex="1"{CheckedOnlyPathSearch} />Искать в названиях каталогов<br />
<!-- END blockOnlyPathSearch -->

<!-- BEGIN blockfilters -->
	Тип искомых файлов:<br />
<!-- BEGIN filters -->
	<input type="checkbox" name="{filters.NAME}" alt="{filters.ALT}" tabindex="{filters.TABINDEX}"{filters.Checked} />{filters.ALT}&nbsp;
<!-- END filters -->
	<br />
<!-- END blockfilters -->

<!-- BEGIN blockOnlyActive -->
	<input type="checkbox" name="OnlyActive" alt="Выводить только активные ссылки" tabindex="{Tab_OnlyActive_Id}"{CheckedOnlyActive} />Выводить только активные ссылки
<!-- END blockOnlyActive -->

	</p>
  <input type="reset" value="Очистить форму" tabindex="{Tab_Reset_Id}" />&nbsp;
	<input type="submit" value='Поиск' tabindex="{Tab_Submit_Id}" />
</form>

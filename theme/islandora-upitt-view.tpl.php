<?php
/**
	* @file
	* Islandora-upitt-view display template.
	*
	* Variables available:
	* - $upitt_date: The defined date corresponding to the Solr field.
	* - $upitt_creator: The defined creator corresponding to the Solr field.
	* - $upitt_lg_thumb: The defined large thumbnail image corresponding to the Solr field.
	*
	* @see template_preprocess_islandora-upitt-view
	*/
?>


		<p class="subtitle-first"> <?php print $upitt_date; ?></p>
		<p class="subtitle"><?php print $upitt_creator; ?></p>

    <div style="float: left; width: 40%;">
    <?php print $upitt_lg_thumb; ?>
    </div>
    <div style="float:right; width:60%;">
    <?php print $metadata; ?>
    </div>




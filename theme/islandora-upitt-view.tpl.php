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

<!--<div id="two-col-left-main">-->

		<p class="subtitle-first"><?php print $upitt_date; ?></p>
		<p class="subtitle"><?php print $upitt_creator; ?></p>
		<br />
	<div id="content">
    <div id="object-pg-left-col">
      <?php if(isset($upitt_viewer_link)): ?>
        <div>
          <?php print $upitt_viewer_link; ?>
        </div>
      <?php endif; ?>
      <?php if(isset($upitt_lg_thumb)): ?>
        <div>
          <?php print $upitt_lg_thumb; ?>
        </div>
      <?php endif; ?>

    </div><!-- /end ojbect-pg-left-col -->

    <div id="object-pg-right-col">
      <?php print $metadata; ?>
    </div><!-- /end ojbect-pg-right-col -->
  </div><!-- /end content -->
<!--</div><!-- /end two-col-left-main -->
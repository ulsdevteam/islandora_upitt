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

<div id="two-col-left-main">
  <p class="subtitle-first"><?php print $upitt_date; ?></p>
  <?php if (isset($upitt_creator)) : ?>
  <p class="subtitle"><?php print $upitt_creator; ?></p>
  <?php endif; ?>
  <p class="subtitle"><?php print $upitt_viewer_link; ?></p>
  <div id="main-content"> 
    <?php if (isset($upitt_object_view)): ?>
    <?php print $upitt_object_view; ?> 
    <?php endif; ?>
    <?php print $upitt_lg_thumb; ?> 

  </div><!-- /end main-content -->
  <div id="sidebar"> 
    <?php print render($page['sidebar']); ?>
    <?php print $metadata; ?>
    <?php print $discovery_links_block; ?>
  </div><!-- /end sidebar -->

  <fieldset class="file_details">
    <legend><span class="fieldset-legend"><?php print t('Resources'); ?></span></legend>
    <table>
      <tr>
        <th><?php print t('Resource Label'); ?></th>
        <th class="right_align"><?php print t('Size'); ?></th>
      </tr>
      <?php foreach($datastreams as $key => $value): ?>
      <tr>
          <td><?php if ($value['download_prohibit_reasons']) : ?>
              <span title="<?php print $value['download_prohibit_reasons']; ?>"><?php print $value['label']; ?></span>
              <?php elseif(isset($value['label_link'])): ?><?php print $value['label_link']; ?><?php endif; ?>
          </td>
          <td class="right_align"><?php if(isset($value['size'])): ?><?php print $value['size']; ?><?php endif; ?></td>
      </tr>
      <?php endforeach; ?>
    </table>
  </fieldset>

</div><!-- /end two-col-left-main -->

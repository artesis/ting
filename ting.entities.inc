<?php

/**
 * @file
 * Entity classes for ting objects and collections.
 */

/**
 * Object representing a relation between TingEntites.
 */
class TingRelation extends DingEntityBase {

  public $type = DingEntityBase::NULL;

  public $uri = DingEntityBase::NULL;

  public $object = DingEntityBase::NULL;

  public function __construct($type, $uri, $object = NULL) {
    parent::__construct();
    $this->properties['type'] = $type;
    $this->properties['uri'] = $uri;
    if ($object) {
      $this->properties['object'] = $object;
    }
  }

  public function getObject() {
    return $this->properties['object'];
  }

}

/**
 * Ting object entity.
 */
class TingEntity extends DingEntity {

  protected $type = DingEntityBase::NULL;

  protected $serieNumber = DingEntityBase::NULL;

  protected $serieTitle = DingEntityBase::NULL;

  protected $record = DingEntityBase::NULL;

  protected $relations = DingEntityBase::NULL;

  protected $localId = DingEntityBase::NULL;
  protected $_localId;

  protected $ownerId = DingEntityBase::NULL;

  protected $creators = DingEntityBase::NULL;
  protected $_creators = array();

  protected $date = DingEntityBase::NULL;
  protected $_date;

  protected $subjects = DingEntityBase::NULL;

  protected $language = DingEntityBase::NULL;

  protected $onlineUrl = DingEntityBase::NULL;

  protected $acSource = DingEntityBase::NULL;

  protected $description = DingEntityBase::NULL;

  protected $contributors = DingEntityBase::NULL;

  protected $isPartOf = DingEntityBase::NULL;

  protected $extent = DingEntityBase::NULL;

  protected $classification = DingEntityBase::NULL;

  protected $isbn = DingEntityBase::NULL;

  protected $descriptionSeries = DingEntityBase::NULL;

  protected $image = array();

  public function getExtent() {
    $extent = $this->get('extent');
    return !empty($extent) ? $extent : FALSE;
  }

  public function getClassification() {
    return $this->get('subject_dk5');
  }

  public function getSubjectDK5Text() {
    $result = array();
    $dk5 = $this->get('subject_dk5text');
    if (!empty($dk5)) {
      if (!is_array($dk5)) {
        $dk5 = array($dk5);
      }
      $result = $dk5;
    }
    return $result;
  }

  public function getSubjectDBCS() {
    $result = array();
    $subjects = $this->get('subject_dbcs');
    if (!empty($subjects)) {
      if (!is_array($subjects)) {
        $subjects = array($subjects);
      }
      $result = $subjects;
    }
    return $result;
  }

  public function getSubjectDBCM() {
    $result = array();
    $subjects = $this->get('subject_dbcm');
    if (!empty($subjects)) {
      if (!is_array($subjects)) {
        $subjects = array($subjects);
      }
      $result = $subjects;
    }
    return $result;
  }

  public function getSubjectDBCN() {
    $result = array();
    $subjects = $this->get('subject_dbcn');
    if (!empty($subjects)) {
      if (!is_array($subjects)) {
        $subjects = array($subjects);
      }
      $result = $subjects;
    }
    return $result;
  }

  public function getSubjectDBCF() {
    $result = array();
    $subjects = $this->get('spatial_dbcf');
    if (!empty($subjects)) {
      if (!is_array($subjects)) {
        $subjects = array($subjects);
      }
      $result = $subjects;
    }
    return $result;
  }

  public function getSpacialDBCF() {
    $result = array();
    $subjects = $this->get('subject_dbcf');
    if (!empty($subjects)) {
      if (!is_array($subjects)) {
        $subjects = array($subjects);
      }
      $result = $subjects;
    }
    return $result;
  }


  public function getIsPartOf() {
    $isPartOf = $this->get('ispartof');
    return !empty($isPartOf) ? $isPartOf : FALSE;
  }

  /**
  * Created this method to avoid core-duplications.
  */
  private function getContributorTypebased($entry) {
    $result = array();
    $contrib = $this->get($entry);
    if (!empty($contrib)) {
      if (!is_array($contrib)) {
        $contrib = array($contrib);
      }
      $result = $contrib;
    }
    return $result;
  }

  /**
  * Return array of fetched contributors.
  */
  public function getContributors() {
    return array_merge(
      $this->getContributorTypebased('contributor'),
      $this->getContributorTypebased('contributor_aut'),
      $this->getContributorTypebased('contributor_dkmed'),
      $this->getContributorTypebased('contributor_ctb'),
      $this->getContributorTypebased('contributor_trl'),
      $this->getContributorTypebased('contributor_edt')
    );
  }

  public function getLocalId() {
    if (empty($this->_localId)) {
      if (empty($this->data)) {
        $this->_localId = NULL;
      }
      else {
        $this->_localId = $this->data->getLocalId();
      }
    }
    return $this->_localId;
  }

  public function getOwnerId() {
    if (empty($this->data)) {
      return NULL;
    }
    return $this->data->getOwnerId();
  }

  public function setLocalId($id) {
    $this->_localId = $id;
  }

  public function setDingEntityId($id) {
    $this->ding_entity_id = $id;
  }

  public function getType() {
    if (empty($this->_type)) {
      $type = $this->get('type_bibdktype');
      $this->_type = is_array($type) ? current($type) : $type;
    }

    return $this->_type;
  }

  /**
   * @return bool
   *  true if this item has many types.
   */
  public function isMultiType() {
    return is_array($this->get('type_bibdktype'));
  }

  public function setType($type) {
    $this->_type = $type;
  }

  public function getTitle() {
    if (empty($this->_title)) {
      $title = $this->getOneOf('title_full', 'title');
      $this->_title = $title;
    }
    return $this->_title;
  }

  public function setTitle($title) {
    $this->_title = $title;
  }

  public function getYear() {
    $year = $this->get('date');

    if (is_array($year)) {
      return current($year);
    }

    return !empty($year) ? $year : FALSE;
  }

  /**
   * Helper function for processing ting_series.
   *
   * @return array|null
   *   Array of series titles.
   */
  private function splitSerie() {
    $series = $this->get('title_series');
    if (!is_array($series)) {
      $series = array($series);
    }

    foreach ($series as $k => $serie) {
      if (strpos($serie, ';')) {
        $serie = explode(';', $serie);
        $series[$k] = array(
          'title' => trim($serie[0]),
          'serie_number' => trim($serie[1]),
        );
      }
      else {
        $series[$k] = array(
          'title' => trim($serie),
        );
      }
    }

    return $series;
  }

  /**
   * Get the serie title.
   *
   * @return array|bool
   *   Array of series titles.
   */
  public function getSerieTitle() {
    $series = $this->splitSerie();
    // If title key of first element is empty, return FALSE.
    if (empty($series[0]['title'])) {
      return FALSE;
    }

    return $series;
  }

  public function getAbstract() {
    $abstract = $this->get('abstract');
    return !empty($abstract) ? $abstract : FALSE;
  }

  public function getRelations() {
    if (!$this->data) {
      return array();
    }

    $relations = $this->data->getRelations();

    $this->relations = array();
    $relation_objects = array();
    $entity_ids = array();

    foreach ($relations as $relation) {
      if (!empty($relation['object'])) {
        $entity_ids[] = $relation['uri'];
      }
    }
    if (!empty($entity_ids)) {
      $objects = entity_load('ting_object', array(), array('ding_entity_id' => $entity_ids));
      foreach ($objects as $object) {
        $relation_objects[$object->id] = $object;
      }
    }

    foreach ($relations as $relation) {
      $this->relations[] = new TingRelation($relation['type'], $relation['uri'], isset($relation_objects[$relation['uri']]) ? $relation_objects[$relation['uri']] : NULL);
    }

    return $this->relations;
  }

  public function getCreators() {
    if (empty($this->_creators)) {
      $creators = $this->getOneOf('creator', 'creator_aut');
      if (empty($creators)) {
        $this->_creators = array();
      }
      elseif (!empty($creators) && !is_array($creators)) {
        $creators = array($creators);
      }
      $this->_creators = $creators;
    }

    return (array) $this->_creators;
  }

  public function setCreators(array $creators) {
    $this->_creators = $creators;
  }

  public function getCreatorMus() {
    return $this->get('creator_mus');
  }

  public function getSubjects() {
    $subjects = $this->get('subject');
    if (!empty($subjects) && !is_array($subjects)) {
      $subjects = array($subjects);
    }
    // Merge other subjects.
    $subjects = array_merge(($subjects != NULL ? $subjects : array()), $this->getSubjectDBCM(), $this->getSubjectDBCF(), $this->getSubjectDBCS(), $this->getSubjectDBCN(), $this->getSpacialDBCF());

    return $subjects;
  }

  public function getLanguage() {
    $language = $this->get('language');
    return !empty($language) ? $language : FALSE;
  }

  public function getLanguageIso() {
    $language = $this->get('language_iso639_2');
    return !empty($language) ? $language : FALSE;
  }

  public function getDate() {
    if (empty($this->_date)) {
      $this->_date = $this->get('date');
    }
    return $this->_date;
  }

  public function setDate($date) {
    $this->_date = $date;
  }

  public function getSubjectGenre() {
    $genre = $this->get('subject_genre');
    if (is_array($genre)) {
      $genre = implode(', ', $genre);
    }

    return !empty($genre) ? $genre : FALSE;
  }

  public function getOnlineUrl() {
    $url = $this->get('identifier_uri');
    if (!empty($url)) {
      drupal_alter('ting_online_url', $url, $this);
      return $url;
    }
  }

  public function getSource() {
    $source = $this->get('source');
    return !empty($source) ? $source : FALSE;
  }

  public function getAcSource() {
    $source = $this->get('ac_source');
    return !empty($source) ? $source : FALSE;
  }

  public function getDescription() {
    $description = $this->get('description');
    return !empty($description) ? $description : FALSE;
  }

  /**
   * Get ISBN numbers of the object.
   *
   * @return array
   */
  public function getIsbn() {
    $isbn = $this->get('identifier_isbn');

    // Nothing to do.
    if (empty($isbn)) {
      return array();
    }

    if (!is_array($isbn)) {
      $isbn = array($isbn);
    }

    // Get ISBN numbers.
    foreach ($isbn as $k => $number) {
      $isbn[$k] = str_replace(array(' ', '-'), '', $number);
    }
    rsort($isbn);
    return $isbn;
  }

  public function getIssn() {
    return $this->get('identifier_issn');
  }

  public function getDescriptionSeries() {
    return $this->get('description_series');
  }

  public function getFormat() {
    return $this->get('format');
  }

  public function getLanguageSpoken() {
    return $this->get('language_spoken');
  }

  public function getLanguageSubtitles() {
    return $this->get('language_subtitles');
  }

  public function getSpatial() {
    return $this->get('spatial');
  }

  public function getHaspartTrack() {
    return $this->get('haspart_track');
  }

  public function getIsreferencedby() {
    return $this->get('isreferencedby');
  }

  public function getVersion() {
    return $this->get('version');
  }

  public function getPublisher() {
    return $this->get('publisher');
  }

  public function getRights() {
    return $this->get('rights');
  }

  public function getAudience() {
    return $this->get('audience');
  }

  public function getAudienceAge() {
    return $this->get('audience_age');
  }

  public function getAudiencePegi() {
    return $this->get('audience_pegi');
  }

  public function getAudienceMedieraad() {
    return $this->get('audience_medieraad');
  }

  public function get($key) {
    if (empty($this->data)) {
      return NULL;
    }
    return $this->data->get($key);
  }

  public function __get($prop) {
    if (method_exists($this, 'get' . ucfirst($prop))) {
      if (module_exists('devel')) {
        kpr(ddebug_backtrace());
      }
      $exception = new Exception('Use getter methor for: ' . $prop);
      watchdog(
        'ting',
        t('Use getter methor for: @prop, <pre>!trace</pre>'),
        array('@prop' => $prop, '!trace' => $exception->getTraceAsString()),
        WATCHDOG_CRITICAL
      );
      throw $exception;
    }

    return parent::__get($prop);
  }

  public function getFormatsAvailable() {
    if (empty($this->data)) {
      return NULL;
    }
    return $this->data->getFormats();
  }

  public function getIdentifier() {
    return $this->get('identifier');
  }

  public function getAcIdentifier() {
    return $this->get('ac_identifier');
  }

  public function getIdentifierUri() {
    return $this->get('identifier_uri');
  }

  /**
   * Set cover image path.
   *
   * @param string $img
   *   Image path.
   * @param string $style
   *   Image style.
   */
  public function setImage($img, $style = '-none-') {
    $this->image[$style] = $img;
  }

  public function getImage($style = '-none-') {
    if (empty($this->image[$style])) {
      return NULL;
    }
    return $this->image[$style];
  }

  /**
   * Get first non-empty value of the fields.
   * Accepts any number of field names as parameters.
   *
   * @return NULL | mixed
   */
  public function getOneOf() {
    $fields = func_get_args();
    $result = NULL;
    foreach ($fields as $field) {
      $value = $this->get($field);
      if (!empty($value)) {
        $result = $value;
        break;
      }
    }
    return $result;
  }

}

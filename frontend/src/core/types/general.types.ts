
// Input element
const inputType = {
    text: "text",
    email: "email",
    password: "password",
    number: "number",
    week: "week",
    date: "date",
    url: "url"
}
export type InputType = keyof typeof inputType;


const inputAutocomplete = {
    off: "off",
    on: "on",
    name: "name",
    honorificPrefix: "honorific-prefix",
    givenName: "given-name",
    additionalName: "additional-name",
    familyName: "family-name",
    honorificSuffix: "honorific-suffix",
    nickname: "nickname",
    email: "email",
    datetimeLocal: "datetime-local",
    username: "username",
    // deepcode ignore NoHardcodedPasswords: It is a type file
    newPassword: "new-password",
    // deepcode ignore NoHardcodedPasswords: It is a type file
    currentPassword: "current-password",
    oneTimeCode: "one-time-code",
    organizationTitle: "organization-title",
    organization: "organization",
    streetAddress: "street-address",
    addressLine1: "address-line1",
    addressLine2: "address-line2",
    addressLine3: "address-line3",
    addressLevel4: "address-level4",
    addressLevel3: "address-level3",
    addressLevel2: "address-level2",
    addressLevel1: "address-level1",
    country: "country",
    countryName: "country-name",
    postalCode: "postal-code",
    ccName: "cc-name",
    ccGivenName: "cc-given-name",
    ccAdditionalName: "cc-additional-name",
    ccFamilyName: "cc-family-name",
    ccNumber: "cc-number",
    ccAxp: "cc-exp",
    ccAxpMonth: "cc-exp-month",
    ccAxpYear: "cc-exp-year",
    ccCsc: "cc-csc",
    ccType: "cc-type",
    transactionCurrency: "transaction-currency",
    transactionAmount: "transaction-amount",
    language: "language",
    bday: "bday",
    bdayDay: "bday-day",
    bdayMonth: "bday-month",
    bdayYear: "bday-year",
    sex: "sex",
    tel: "tel",
    telCountryCode: "tel-country-code",
    telNational: "tel-national",
    telAreaCode: "tel-area-code",
    telLocal: "tel-local",
    telExtension: "tel-extension",
    impp: "impp",
    url: "url",
    photo: "photo",
    webauthn: "webauthn"
}
export type InputAutocomplete = keyof typeof inputAutocomplete;


/**
 * Is used for the input filed and it is NOT a type. But used for defining the type "InputSize"
 * @type {{ default: string; small: string; big: string; }}
 */
export const inputSizes = {
    default: `
        px-2
        py-1.5
    `,
    small: `
        px-1.5
        py-1
    `
    ,
    big: `
        px-2
        py-12
    `
}
export type InputSize = keyof typeof inputSizes;

const inputAutoComplete = {
    off: "off",
    on: "on"
}
export type InputAutoComplete = keyof typeof inputAutoComplete;